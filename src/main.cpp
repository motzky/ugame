#include <cmath>
#include <format>
#include <iostream>
#include <memory>
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
#include "game/aabb.h"
#include "game/chain.h"
#include "graphics/cube_map.h"
#include "graphics/material.h"
#include "graphics/renderer.h"
#include "graphics/scene.h"
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
#include "tlv/tlv_entry.h"
#include "tlv/tlv_reader.h"
#include "window.h"

struct GameTransformState
{
    const game::Camera *camera;
    game::Vector3 camera_last_position;
};

constexpr auto CameraDelta = [](const game::Vector3 &in, const GameTransformState &state) -> game::TransformerResult
{ return {in + state.camera->position() - state.camera_last_position}; };

constexpr auto Invert = [](const game::Vector3 &in, const GameTransformState &) -> game::TransformerResult
{ return {-in}; };

constexpr auto CheckVisible = [](const game::Vector3 &in, const GameTransformState &) -> game::TransformerResult
{
    return {-in};
};

struct TransformedEntity
{
    game::Entity entity;
    game::AABB bounding_box;
    std::unique_ptr<game::ChainBase<GameTransformState>> transformer_chain;
};

auto main(int argc, char **argv) -> int
{
    game::log::info("starting game...");

    try
    {
        game::ensure(argc >= 2, "{} <root_path>", argv[0]);

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
                                   0.001f,
                                   500.f};

        auto resource_loader = game::ResourceLoader{argv[1]};

        auto mesh_loader = game::MeshLoader{resource_loader};

        auto tex_data = std::optional<game::TextureDescription>{};

        game::log::info("loading resources...");
        const auto tlv_file = resource_loader.load("resources");

        const auto reader = game::TlvReader{tlv_file.as_bytes()};

        game::log::info("Creating GL textures...");

        auto sampler = game::TextureSampler{};
        auto albedo_tex = game::Texture{reader, "barrel_base_albedo", &sampler};
        auto spec_map = game::Texture{reader, "barrel_metallic", &sampler};
        auto normal_map = game::Texture{reader, "barrel_normal_ogl", &sampler};

        game::log::info("Creating materials...");

        const game::Texture *textures[]{&albedo_tex, &spec_map, &normal_map};

        const auto vertex_shader_file = resource_loader.load("simple.vert");
        const auto fragment_shader_file = resource_loader.load("simple.frag");

        const auto vertex_shader = game::Shader{vertex_shader_file.as_string(), game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{fragment_shader_file.as_string(), game::ShaderType::FRAGMENT};
        auto material = game::Material{vertex_shader, fragment_shader};

        game::log::info("Loading meshes...");
        const auto mesh = game::Mesh{reader, "Cylinder.014"};

        auto renderer = game::Renderer{resource_loader, mesh_loader, width, height};

        const auto checker_vertex_shader_file = resource_loader.load("checker.vert");
        const auto checker_fragment_shader_file = resource_loader.load("checker.frag");

        const auto checker_vertex_shader = game::Shader{checker_vertex_shader_file.as_string(), game::ShaderType::VERTEX};
        const auto checker_fragment_shader = game::Shader{checker_fragment_shader_file.as_string(), game::ShaderType::FRAGMENT};
        auto floor_material = game::Material{checker_vertex_shader, checker_fragment_shader};

        const auto floor_sampler = game::TextureSampler{};
        const auto floor_mesh = game::Mesh{mesh_loader.cube()};
        const auto floor_texture = game::Texture{
            game::TextureDescription{
                .name = "white",
                .format = game::TextureFormat::RGB,
                .usage = game::TextureUsage::SRGB,
                .width = 1u,
                .height = 1u,
                .data{static_cast<std::byte>(0xff), static_cast<std::byte>(0xff), static_cast<std::byte>(0xff)}},
            &floor_sampler};

        const game::Texture *floor_textures[]{&floor_texture};
        auto floor_entity = game::Entity{&floor_mesh, &floor_material, {0.f, -2.f, 0}, {100.f, 1.f, 100.f}, floor_textures};

        auto entities = std::vector<TransformedEntity>{};
        entities.emplace_back(game::Entity{&mesh, &material, {-5.f, 0.f, 0.f}, {0.4f}, {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}, textures},
                              game::AABB{{-5.6f, -.75f, -.6f}, {-4.4f, .75f, .6f}},
                              std::make_unique<game::Chain<GameTransformState, CameraDelta, Invert>>());
        entities.emplace_back(game::Entity{&mesh, &material, {}, {0.4f}, {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}, textures},
                              game::AABB{{-.6f, -.75f, -.6f}, {.6f, .75f, .6f}},
                              std::make_unique<game::Chain<GameTransformState>>());
        entities.emplace_back(game::Entity{&mesh, &material, {5.f, 0.f, 0.f}, {0.4f}, {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}, textures},
                              game::AABB{{4.4f, -.75f, -.6f}, {5.6f, .75f, .6f}},
                              std::make_unique<game::Chain<GameTransformState, CameraDelta>>());

        auto scene = game::Scene{
            .entities = entities |
                        std::views::transform([](const auto &e)
                                              { return std::addressof(e.entity); }) |
                        std::ranges::to<std::vector>(),
            .ambient = {.r = .2f, .g = .2f, .b = .2f},
            .directional = {.direction = {-1.f, -1.f, -1.f}, .color = {.r = .3f, .g = .3f, .b = .3f}},
            .points = {{.position = {5.f, 3.f, 0.f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}, //
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f},
                       {.position = {-5.f, 3.f, 0.f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}, //
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f},
                       {.position = {-5.f, 3.f, 0.f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}, //
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f}},
            .debug_lines = {}};

        scene.entities.push_back(&floor_entity);

        auto skybox = game::CubeMap{
            reader,
            {"skybox_right", "skybox_left", "skybox_top", "skybox_bottom", "skybox_front", "skybox_back"}};

        auto skybox_sampler = game::TextureSampler{};

        auto key_state = std::unordered_map<game::Key, bool>{};

        auto gamma = 2.2f;

        auto show_debug = false;
        const auto debug_ui = game::DebugUi(window.native_handle(), scene, camera, gamma);

        auto show_physics_debug = false;

        auto state = GameTransformState{.camera = &camera, .camera_last_position = camera.position()};

        auto running = true;

        while (running)
        {
#pragma region EventHandling
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
                            else if (arg.key() == game::Key::F2 && arg.state() == game::KeyState::UP)
                            {
                                show_physics_debug = !show_physics_debug;
                            }
                        }
                        else if constexpr (std::same_as<T, game::MouseEvent>)
                        {
                            if (!show_debug)
                            {
                                // game::log::debug("{}", arg);
                                static constexpr auto sensitivity = float{0.005f};
                                camera.adjust_pitch(arg.delta_y() * sensitivity);
                                camera.adjust_yaw(-arg.delta_x() * sensitivity);
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
#pragma endregion

#pragma region Camera Control
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

            walk_direction.y = 0.f;

            const auto speed = key_state[game::Key::LSHIFT] ? 30.f : 10.f;
            camera.translate(game::Vector3::normalize(walk_direction) * (speed / 60.f));
            camera.update();
#pragma endregion

            auto debug_line_data = std::vector<game::LineData>{};

            for (const auto &[transformed_entity, light] : std::views::zip(entities, scene.points))
            {
                auto &[entity, aabb, transformer] = transformed_entity;
                const auto enitiy_delta = transformer->go({}, state);
                entity.translate(enitiy_delta);

                aabb.min += enitiy_delta;
                aabb.max += enitiy_delta;

                const auto position = entity.position();
                light.position = {position.x, 5.f, position.z};

#pragma region DrawAABB

                debug_line_data.push_back({aabb.max, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.min.x, aabb.max.y, aabb.max.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.min.x, aabb.max.y, aabb.max.z}, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.min.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.min.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.max.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.max.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({aabb.max, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({aabb.max, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.max.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.min.x, aabb.max.y, aabb.max.z}, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.min.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.max.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.max.x, aabb.min.y, aabb.min.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.min.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({aabb.min, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({aabb.min, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.min.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.min.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.max.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.max.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({{aabb.max.x, aabb.min.y, aabb.min.z}, {0.f, 1.f, 0.f}});

                debug_line_data.push_back({{aabb.max.x, aabb.min.y, aabb.min.z}, {0.f, 1.f, 0.f}});
                debug_line_data.push_back({aabb.min, {0.f, 1.f, 0.f}});
#pragma endregion
            }

            if (show_physics_debug)
            {
                scene.debug_lines = game::DebugLines{debug_line_data};
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

            state.camera_last_position = camera.position();
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