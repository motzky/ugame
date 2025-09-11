#include <cmath>
#include <format>
#include <iostream>
#include <numbers>
#include <print>
#include <random>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <variant>

#include "camera.h"
#include "core/exception.h"
#include "debug_ui.h"
#include "ensure.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "events/stop_event.h"
#include "graphics/cube_map.h"
#include "graphics/material.h"
#include "graphics/renderer.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "loaders/mesh_loader.h"
#include "loaders/resource_loader.h"
#include "log.h"
#include "physics/box_shape.h"
#include "physics/cylinder_shape.h"
#include "physics/debug_renderer.h"
#include "physics/physics_sytem.h"
#include "physics/rigid_body.h"
#include "physics/sphere_shape.h"
#include "primitives/entity.h"
#include "scene.h"
#include "tlv/tlv_entry.h"
#include "tlv/tlv_reader.h"
#include "window.h"

struct GameEntity
{
    game::Entity render_entity;
    game::RigidBody physics_entity;
};

auto main(int argc, char **argv) -> int
{

    try
    {
        game::ensure(argc >= 2, "{} <root_path>", argv[0]);

        game::log::info("hello world");

        auto width = 1920u;
        auto height = 1080u;
        // auto width = 1280u;
        // auto height = 720u;
        game::log::info("Creating Window {}x{}...", width, height);
        auto window = game::Window{width, height};

        auto ps = game::PhysicsSystem{};
        const auto floor_shape = ps.create_shape<game::BoxShape>(game::Vector3{50.f, 1.f, 50.f});
        [[maybe_unused]] auto floor = ps.create_rigid_body(floor_shape, {0.f, -1.f, 0.f}, game::RigidBodyType::STATIC);

        auto resource_loader = game::ResourceLoader{argv[1]};

        auto mesh_loader = game::MeshLoader{resource_loader};

        auto tex_data = std::optional<game::TextureDescription>{};

        game::log::info("loading resources...");
        const auto tlv_file = resource_loader.load("resources");

        const auto reader = game::TlvReader{tlv_file.as_bytes()};

        game::log::info("Creating GL textures...");

        auto sampler = game::TextureSampler{};
        auto albedo_tex = game::Texture{reader, "barrel_base_albedo"};
        auto spec_map = game::Texture{reader, "barrel_metallic"};
        auto normal_map = game::Texture{reader, "barrel_normal_ogl"};

        game::log::info("Creating materials...");

        const game::Texture *textures[]{&albedo_tex, &spec_map, &normal_map};
        const game::TextureSampler *samplers[]{&sampler, &sampler, &sampler};
        auto tex_samp = std::views::zip(textures, samplers) | std::ranges::to<std::vector>();

        const auto vertex_shader_file = resource_loader.load("simple.vert");
        const auto fragment_shader_file = resource_loader.load("simple.frag");

        const auto vertex_shader = game::Shader{vertex_shader_file.as_string(), game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{fragment_shader_file.as_string(), game::ShaderType::FRAGMENT};
        auto material = game::Material{vertex_shader, fragment_shader};

        game::log::info("Loading meshes...");
        const auto mesh = game::Mesh{reader, "Cylinder.014"};

        auto renderer = game::Renderer{resource_loader, mesh_loader, width, height};

        auto entities = std::vector<GameEntity>{};

        const auto cylinder_shape = ps.create_shape<game::CylinderShape>(1.f, .5f);

        [[maybe_unused]] auto rd = std::random_device{};
        [[maybe_unused]] auto gen = std::mt19937{rd()};
        // [[maybe_unused]] auto dis = std::uniform_real_distribution(-5.f, 5.f);
        [[maybe_unused]] auto dis = std::uniform_real_distribution(1.f, 11.f);

        for (auto i = -10; i < 10; ++i)
        {
            for (auto j = -10; j < 10; ++j)
            {
                auto x = static_cast<float>(i) * 2.5f;
                // auto y = 0.f;
                auto y = 1.f + dis(gen);
                auto z = static_cast<float>(j) * 2.5f;
                entities.push_back({{&mesh,
                                     &material,
                                     // game::Vector3{static_cast<float>(i) * 2.5f, dis(gen), static_cast<float>(j) * 2.5f},
                                     game::Vector3{x, y, z},
                                     game::Vector3{.4f},
                                     tex_samp},
                                    ps.create_rigid_body(cylinder_shape, {x, y, z}, game::RigidBodyType::DYNAMIC)});
            }
        }

        auto scene = game::Scene{
            .entities = entities |
                        std::views::transform([](const auto &e)
                                              { return &e.render_entity; }) |
                        std::ranges::to<std::vector>(),
            .ambient = {.r = .2f, .g = .2f, .b = .2f},
            .directional = {.direction = {-1.f, -1.f, -1.f}, .color = {.r = .3f, .g = .3f, .b = .3f}},
            .points = {{.position = {5.f, 5.f, 0.f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}, //
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07,
                        .quad_attenuation = 0.017},
                       {.position = {-5.f, 5.f, 0.f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}, //
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07,
                        .quad_attenuation = 0.017}},
            .debug_lines = {}};

        auto camera = game::Camera{{0.f, 5.f, 50.f},
                                   {0.f, 0.f, 0.f},
                                   {0.f, 1.f, 0.f},
                                   std::numbers::pi_v<float> / 4.f,
                                   static_cast<float>(window.width()),
                                   static_cast<float>(window.height()),
                                   0.001f,
                                   100.f};

        auto skybox = game::CubeMap{
            reader,
            {"skybox_right", "skybox_left", "skybox_top", "skybox_bottom", "skybox_front", "skybox_back"}};

        auto skybox_sampler = game::TextureSampler{};

        auto key_state = std::unordered_map<game::Key, bool>{};

        auto gamma = 2.2f;

        auto show_debug = false;
        const auto debug_ui = game::DebugUi(window.native_handle(), scene, camera, gamma);

        // const auto sphere_shape = ps.create_shape<game::SphereShape>(5.f);
        // [[maybe_unused]] auto sphere = ps.create_rigid_body(sphere_shape, {0.f, 100.f, 0.f}, game::RigidBodyType::DYNAMIC);

        ps.optimize();

        auto running = true;

        while (running)
        {
            auto event = window.pump_event();
            while (event && running)
            {
                std::visit(
                    [&](auto &&arg)
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::same_as<T, game::StopEvent>)
                        {
                            running = false;
                        }
                        else if constexpr (std::same_as<T, game::KeyEvent>)
                        {
                            // game::log::debug("{}", arg);

                            key_state[arg.key()] = arg.state() == game::KeyState::DOWN;
                            if (arg.key() == game::Key::F1 && arg.state() == game::KeyState::UP)
                            {
                                show_debug = !show_debug;
                                window.show_cursor(show_debug);
                            }
                        }
                        else if constexpr (std::same_as<T, game::MouseEvent>)
                        {
                            if (!show_debug)
                            {
                                static constexpr auto sensitivity = float{0.005f};
                                camera.adjust_pitch(arg.delta_y() * sensitivity);
                                camera.adjust_yaw(arg.delta_x() * sensitivity);
                            }
                        }
                        else if constexpr (std::same_as<T, game::MouseButtonEvent>)
                        {
                            debug_ui.add_mouse_event(arg);
                        }
                    },
                    *event);
                event = window.pump_event();
            }

            auto walk_direction = game::Vector3{0.f};

            if (key_state[game::Key::D] || key_state[game::Key::RIGHT_ARROW])
            {
                walk_direction += camera.right();
            }
            if (key_state[game::Key::A] || key_state[game::Key::LEFT_ARROW])
            {
                walk_direction -= camera.right();
            }
            if (key_state[game::Key::W] || key_state[game::Key::UP_ARROW])
            {
                walk_direction += camera.direction();
            }
            if (key_state[game::Key::S] || key_state[game::Key::DOWN_ARROW])
            {
                walk_direction -= camera.direction();
            }
            if (key_state[game::Key::SPACE])
            {
                walk_direction += camera.up();
            }
            if (key_state[game::Key::LCTRL])
            {
                walk_direction -= camera.up();
            }

            const auto speed = key_state[game::Key::LSHIFT] ? 10.f : 3.f;
            camera.translate(game::Vector3::normalize(walk_direction) * (speed / 60.f));
            camera.update();

            ps.update();
            for (auto &[render, physics] : entities)
            {
                auto pos = physics.position();
                render.set_position(pos);
            }
            if (show_debug)
            {
                scene.debug_lines = {ps.debug_renderer().lines()};
            }
            else
            {
                scene.debug_lines.reset();
            }

            renderer.render(camera, scene, skybox, sampler, gamma);
            if (show_debug)
            {
                debug_ui.render();
            }
            window.swap();
        }
    }
    catch (const game::Exception &err)
    {
        std::println(std::cerr, "{}", err);
    }
    catch (...)
    {
        std::println(std::cerr, "unknown exception");
    }

#ifndef WIN32
    glfwTerminate();
#endif

    return 0;
}