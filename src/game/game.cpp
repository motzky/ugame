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
#include "math/aabb.h"
#include "math/frustum_plane.h"
#include "messaging/message_bus.h"
#include "primitives/entity.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "tlv/tlv_entry.h"
#include "tlv/tlv_reader.h"
#include "utils/exception.h"
#include "window.h"

namespace
{
    auto intersects_frustum(const game::AABB &aabb, const std::array<game::FrustumPlane, 6u> &planes) -> bool
    {
        for (const auto &plane : planes)
        {

            auto pos_vert = aabb.min;
            if (plane.normal.x >= 0)
            {
                pos_vert.x = aabb.max.x;
            }
            if (plane.normal.y >= 0)
            {
                pos_vert.y = aabb.max.y;
            }
            if (plane.normal.z >= 0)
            {
                pos_vert.z = aabb.max.z;
            }

            if (game::Vector3::dot(plane.normal, pos_vert) + plane.distance < 0.f)
            {
                return false;
            }
        }
        return true;
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

    Game::Game(std::uint32_t width, std::uint32_t height)
        : _running(true),
          _levels{},
          _level_num{0ul},
          _message_bus{},
          _window{width, height},
          _player{_message_bus, create_camera(_window)}
    {
        _message_bus.subscribe(messaging::MessageType::LEVEL_COMPLETE, this);
    }

    Game::~Game()
    {
    }

    auto Game::run(std::string_view resource_root) -> void
    {
        auto resource_loader = game::ResourceLoader{resource_root};
        auto mesh_loader = game::MeshLoader{resource_loader};
        auto resource_cache = game::DefaultCache{};

        const auto *sampler = resource_cache.insert<game::TextureSampler>("default");

        game::log::info("loading resources...");
        const auto tlv_file = resource_loader.load("resources");

        const auto reader = game::TlvReader{tlv_file.as_bytes()};

        game::log::info("Loading meshes...");
        resource_cache.insert<Mesh>("barrel", reader, "Cylinder.014");
        resource_cache.insert<Mesh>("floor", mesh_loader.cube());

        game::log::info("Creating materials...");

        const auto vertex_shader_file = resource_loader.load("simple.vert");
        const auto fragment_shader_file = resource_loader.load("barrel.frag");

        const auto vertex_shader = game::Shader{vertex_shader_file.as_string(), game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{fragment_shader_file.as_string(), game::ShaderType::FRAGMENT};
        resource_cache.insert<Material>("barrel_material", vertex_shader, fragment_shader);

        auto renderer = game::Renderer{resource_loader, mesh_loader, _window.width(), _window.height()};

        const auto checker_vertex_shader_file = resource_loader.load("simple.vert");
        const auto checker_fragment_shader_file = resource_loader.load("checker.frag");

        const auto checker_vertex_shader = game::Shader{checker_vertex_shader_file.as_string(), game::ShaderType::VERTEX};
        const auto checker_fragment_shader = game::Shader{checker_fragment_shader_file.as_string(), game::ShaderType::FRAGMENT};
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

        _levels.push_back(std::make_unique<levels::LuaLevel>(resource_loader, "level_apple.lua", resource_cache, reader, _player, _message_bus));
        _levels.push_back(std::make_unique<levels::LuaLevel>(resource_loader, "level_kiwi.lua", resource_cache, reader, _player, _message_bus));

        _levels[_level_num]->restart();
        _player.restart();

        auto gamma = 2.2f;

        auto show_debug = false;
        // const auto debug_ui = game::DebugUi(window.native_handle(), level.scene(), player.camera(), gamma);

        auto show_physics_debug = false;

        auto debug_wireframe_renderer = game::ShapeWireframeRenderer{};

        while (_running)
        {
            auto *level = _levels[_level_num].get();

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

                debug_wireframe_renderer.draw(entity->bounding_box());
            }

            if (show_physics_debug)
            {
                level->scene().debug_lines = game::DebugLines{debug_wireframe_renderer.yield()};
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
        if (_level_num >= _levels.size())
        {
            log::info("YOU WIN !");
            // _running = false;
            _level_num = 0;
        }

        if (_running)
        {
            _levels[_level_num]->restart();
            _player.restart();
        }
    }
}