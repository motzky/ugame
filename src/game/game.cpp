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
#include "core/exception.h"
#include "graphics/debug_ui.h"
// #include "utils/ensure.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "events/stop_event.h"
#include "game/chain.h"
#include "game/levels/level.h"
#include "game/levels/level_apple.h"
#include "game/levels/level_kiwi.h"
#include "game/levels/lua_level.h"
#include "game/player.h"
#include "graphics/cube_map.h"
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
#include "window.h"

namespace game
{
    Game::Game()
        : _running(true),
          _levels{},
          _level_num{0ul}
    {
    }

    Game::~Game()
    {
#ifndef WIN32
        ::glfwTerminate();
#endif
    }

    auto Game::run(std::string_view resource_root) -> void
    {
        auto bus = game::messaging::MessageBus{};

        bus.subscribe(messaging::MessageType::LEVEL_COMPLETE, this);

        auto width = 1920u;
        auto height = 1080u;
        // auto width = 1280u;
        // auto height = 720u;
        game::log::info("Creating Window {} x {}...", width, height);
        auto window = game::Window{width, height};

        auto camera = game::Camera{{0.f, 2.f, 20.f},
                                   {0.f, 0.f, 0.f},
                                   {0.f, 1.f, 0.f},
                                   std::numbers::pi_v<float> / 4.f,
                                   static_cast<float>(window.width()),
                                   static_cast<float>(window.height()),
                                   0.1f,
                                   500.f};

        auto player = game::Player{bus, std::move(camera)};

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

        auto renderer = game::Renderer{resource_loader, mesh_loader, width, height};

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

        // _levels.push_back(std::make_unique<levels::LevelApple>(resource_cache, reader, player, bus));
        // _levels.push_back(std::make_unique<levels::LevelKiwi>(resource_cache, reader, player, bus));

        _levels.push_back(std::make_unique<levels::LuaLevel>(resource_loader, "level_apple.lua", resource_cache, reader, player, bus));

        _levels[_level_num]->restart();

        auto gamma = 2.2f;

        auto show_debug = false;
        // const auto debug_ui = game::DebugUi(window.native_handle(), level.scene(), player.camera(), gamma);

        auto show_physics_debug = false;

        auto debug_wireframe_renderer = game::ShapeWireframeRenderer{};

        while (_running)
        {
            auto *level = _levels[_level_num].get();

#pragma region EventHandling
            auto event = window.pump_event();
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

                            bus.post_key_press(arg);

                            if (arg.key() == game::Key::F1 && arg.state() == game::KeyState::UP)
                            {
                                show_debug = !show_debug;
                                window.show_cursor(show_debug);
                                player.set_flying(show_debug);
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
                                bus.post_mouse_move(arg);
                            }
                        }
                        else if constexpr (std::same_as<T, game::MouseButtonEvent>)
                        {
                            bus.post_mouse_button(arg);
                            // debug_ui.add_mouse_event(arg);
                        }
                    },
                    *event);
                event = window.pump_event();
            }
#pragma endregion

            player.update();
            level->update(player);

            if (show_physics_debug)
            {
                for (const auto &entity : level->entities())
                {
                    debug_wireframe_renderer.draw(entity.bounding_box);
                }
                level->scene().debug_lines = game::DebugLines{debug_wireframe_renderer.yield()};
            }
            else
            {
                level->scene().debug_lines.reset();
            }

            renderer.render(player.camera(), level->scene(), gamma);
            if (show_debug)
            {
                // debug_ui.render();
            }
            window.swap();
        }
    }

    auto Game::handle_level_complete(const std::string_view &level_name) -> void
    {
        game::log::info("level complete: {}", level_name);
        _level_num++;
        if (_level_num >= _levels.size())
        {
            log::info("YOU WIN !");
            _running = false;
        }
    }
}