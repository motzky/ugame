#include "game/game.h"

#include <array>
#include <cmath>
#include <memory>
#include <numbers>
#include <print>
#include <random>
#include <ranges>
#include <utility>
#include <variant>

#ifndef WIN32
#include <GLFW/glfw3.h>
#endif

#include "camera.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "events/stop_event.h"
#include "game/levels/level.h"
#include "game/levels/lua_level.h"
#include "game/player.h"
#include "graphics/cube_map.h"
#include "graphics/debug_ui.h"
#include "graphics/material.h"
#include "graphics/renderer.h"
#include "graphics/scene.h"
#include "graphics/shader.h"
#include "graphics/shape_wireframe_renderer.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "loaders/mesh_loader.h"
#include "log.h"
#include "math/frustum_plane.h"
#include "messaging/message_bus.h"
#include "physics/box_shape.h"
#include "physics/transformed_shape.h"
#include "primitives/entity.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "tlv/tlv_entry.h"
#include "tlv/tlv_reader.h"
#include "utils/decompress.h"
#include "utils/exception.h"
#include "window.h"

using namespace std::string_view_literals;

namespace
{
    auto intersects_frustum(const game::TransformedShape &bounding_box, const std::array<game::FrustumPlane, 6u> &planes) -> bool
    {
        game::expect(bounding_box.shape()->type() == game::ShapeType::BOX, "Not a bounding BOX");

        const auto *box_shape = dynamic_cast<const game::BoxShape *>(bounding_box.shape());
        const auto position = bounding_box.transform().position;

        const auto min = position - box_shape->dimensions();
        const auto max = position + box_shape->dimensions();

        for (const auto &plane : planes)
        {
            auto pos_vert = min;
            if (plane.normal.x >= 0)
            {
                pos_vert.x = max.x;
            }
            if (plane.normal.y >= 0)
            {
                pos_vert.y = max.y;
            }
            if (plane.normal.z >= 0)
            {
                pos_vert.z = max.z;
            }

            if (game::Vector3::dot(plane.normal, pos_vert) + plane.distance < 0.f)
            {
                return false;
            }
        }
        return true;
    }

    auto get_uint_arg(const std::vector<std::string_view> &args, std::string_view arg_name, std::uint32_t defval = 0u) -> std::uint32_t
    {
        const auto arg = std::ranges::find(args, arg_name);
        if (arg == std::ranges::cend(args))
        {
            return defval;
        }

        const auto index = static_cast<std::size_t>(std::ranges::distance(std::ranges::cbegin(args), arg));
        if (index + 1u >= args.size())
        {
            return defval;
        }

        return std::stol(args[index + 1].data());
    }
}

namespace game
{
    auto create_camera(const Window &window) -> Camera
    {
        return {{0.f, 2.f, 20.f},
                {0.f, 0.f, 0.f},
                {0.f, 1.f, 0.f},
                std::numbers::pi_v<float> / 4.f,
                static_cast<float>(window.width()),
                static_cast<float>(window.height()),
                0.1f,
                500.f};
    }

    Game::Game(const std::vector<std::string_view> &args)
        : _running(true),
          _level_names{
              "level_papaya.lua",
              "level_pear.lua",
              "level_banana.lua",
              "level_apple.lua",
              "level_kiwi.lua",
              "level_mango.lua",
          },
          _level{},
          _level_num{0ul},
          _message_bus{},
          _window{
              get_uint_arg(args, "-width"sv, 1920u),
              get_uint_arg(args, "-height"sv, 1080u),
              get_uint_arg(args, "-x"sv),
              get_uint_arg(args, "-y"sv)},
          _player{_message_bus, create_camera(_window)}
    {
        _message_bus.subscribe(messaging::MessageType::LEVEL_COMPLETE, this);
    }

    Game::~Game() = default;

    auto Game::run(std::string_view resource_root) -> void
    {
        auto mesh_loader = game::MeshLoader{};
        auto resource_cache = game::DefaultCache{};

        const auto *sampler = resource_cache.insert<game::TextureSampler>("default");

        game::log::info("loading resources...");
        auto resource_loader = game::ResourceLoader{resource_root};

        game::log::info("decompressing...");
        const auto tlv_file = [&resource_loader]
        {
            const auto compressed_tlv_file = resource_loader.load("resources");
            return game::decompress(compressed_tlv_file.as_bytes());
        }();

        const auto reader = game::TlvReader{tlv_file};

        game::log::info("Loading meshes...");
        resource_cache.insert<Mesh>("barrel", reader, "Cylinder.014");
        resource_cache.insert<Mesh>("floor", mesh_loader.cube());

        game::log::info("Creating materials...");

        const auto vertex_shader_file = TlvReader::get_text_file(reader, "simple.vert");
        const auto fragment_shader_file = TlvReader::get_text_file(reader, "barrel.frag");

        const auto vertex_shader = game::Shader{vertex_shader_file.data, game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{fragment_shader_file.data, game::ShaderType::FRAGMENT};
        resource_cache.insert<Material>("barrel_material", vertex_shader, fragment_shader);

        const auto checker_vertex_shader_file = TlvReader::get_text_file(reader, "simple.vert");
        const auto checker_fragment_shader_file = TlvReader::get_text_file(reader, "checker.frag");
        const auto checker_vertex_shader = game::Shader{checker_vertex_shader_file.data, game::ShaderType::VERTEX};
        const auto checker_fragment_shader = game::Shader{checker_fragment_shader_file.data, game::ShaderType::FRAGMENT};
        resource_cache.insert<Material>("floor_material", checker_vertex_shader, checker_fragment_shader);

        game::log::info("Creating GL textures...");

        resource_cache.insert<game::Texture>("barrel_albedo", reader, "barrel_base_albedo", sampler);
        resource_cache.insert<game::Texture>("barrel_specular", reader, "barrel_metallic", sampler);
        resource_cache.insert<game::Texture>("barrel_normal", reader, "barrel_normal_ogl", sampler);

        resource_cache.insert<Texture>(
            "floor_albedo",
            game::TextureDescription{
                .name = "white",
                .format = game::TextureFormat::RGB,
                .usage = game::TextureUsage::SRGB,
                .width = 1u,
                .height = 1u,
                .data{static_cast<std::byte>(0xff), static_cast<std::byte>(0xff), static_cast<std::byte>(0xff)}},
            sampler);

        auto renderer = game::Renderer{reader, mesh_loader, _window.width(), _window.height()};

        auto gamma = 2.2f;

        auto show_debug = false;
        // const auto debug_ui = game::DebugUi(window.native_handle(), level.scene(), player.camera(), gamma);

        auto show_physics_debug = false;

        auto debug_wireframe_renderer = game::ShapeWireframeRenderer{};

        auto curernt_level = _level_num;

        while (_running)
        {
            if (_level == nullptr || curernt_level != _level_num)
            {
                _player.restart();
                _level.reset();
                _level = std::make_unique<levels::LuaLevel>(_level_names[_level_num], resource_cache, reader, _player, _message_bus);
                _level->restart();
                curernt_level = _level_num;

                _window.set_title(_level_names[_level_num]);
            }
            auto *level = _level.get();

#pragma region EventHandling
            auto event = _window.pump_event();
            while (event && _running)
            {
                std::visit(
                    [&](auto &&arg)
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::same_as<T, game::StopEvent>)
                        {
                            _running = false;
                        }
                        else if constexpr (std::same_as<T, game::KeyEvent>)
                        {
                            // game::log::debug("{}", arg);

                            _message_bus.post_key_press(arg);

                            if (arg.key() == game::Key::F1 && arg.state() == game::KeyState::UP)
                            {
                                show_debug = !show_debug;
                                _window.show_cursor(show_debug);
                                _player.set_flying(show_debug);
                                level->set_show_debug(show_debug);
                            }
                            else if (arg.key() == game::Key::F2 && arg.state() == game::KeyState::UP)
                            {
                                show_physics_debug = !show_physics_debug;
                            }
                        }
                        else if constexpr (std::same_as<T, game::MouseEvent>)
                        {
                            if (!show_debug)
                            {
                                _message_bus.post_mouse_move(arg);
                            }
                        }
                        else if constexpr (std::same_as<T, game::MouseButtonEvent>)
                        {
                            _message_bus.post_mouse_button(arg);
                            // debug_ui.add_mouse_event(arg);
                        }
                    },
                    *event);
                event = _window.pump_event();
            }
#pragma endregion

            _player.update();
            level->update(_player);

            for (auto &entity : level->scene().entities)
            {
                entity->set_visibility(intersects_frustum(entity->bounding_box(), _player.camera().frustum_planes()));

                entity->bounding_box().draw(level->physics().debug_renderer());
            }

            if (show_physics_debug)
            {
                auto lines = debug_wireframe_renderer.yield();
                for (const auto line : level->physics().debug_renderer().lines())
                {
                    lines.push_back(line);
                }
                level->scene().debug_lines = game::DebugLines{lines};

                level->physics().debug_renderer().clear();
            }
            else
            {
                level->scene().debug_lines.reset();
            }

            renderer.render(_player.camera(), level->scene(), gamma);
            if (show_debug)
            {
                // debug_ui.render();
            }
            _window.swap();
        }
    }

    auto Game::handle_level_complete(const std::string_view &level_name) -> void
    {
        game::log::info("level complete: {}", level_name);
        _level_num++;
        if (_level_num >= _level_names.size())
        {
            log::info("YOU WIN !");
            // _running = false;
            _level_num = 0;
        }
    }
}