#include <format>
#include <iostream>
#include <numbers>
#include <cmath>
#include <print>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <variant>

#include "camera.h"
#include "entity.h"
#include "ensure.h"
#include "event.h"
#include "exception.h"
#include "key_event.h"
#include "log.h"
#include "material.h"
#include "mouse_event.h"
#include "renderer.h"
#include "resource_loader.h"
#include "texture.h"
#include "texture_sampler.h"
#include "scene.h"
#include "shader.h"
#include "stop_event.h"
#include "window.h"

auto main(int argc, char **argv) -> int
{

    try
    {
        game::ensure(argc >= 2, "{} <root_path>", argv[0]);

        game::log::info("hello world");

        auto window = game::Window{800u, 600u};

        auto resource_loader = game::ResourceLoader{argv[1]};

        auto texture = game::Texture{resource_loader.load_binary("container2.png"), 500u, 500u};
        auto spec_map = game::Texture{resource_loader.load_binary("container2_specular.png"), 500u, 500u};
        auto sampler = game::TextureSampler{};

        const auto vertex_shader = game::Shader{resource_loader.load_string("simple.vert"), game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{resource_loader.load_string("simple.frag"), game::ShaderType::FRAGMENT};
        auto material = game::Material{vertex_shader, fragment_shader};
        const auto mesh = game::Mesh{};

        const auto renderer = game::Renderer{};

        auto entities = std::vector<game::Entity>{};

        const auto tex_ptrs = std::vector<const game::Texture *>{&texture, &spec_map};

        for (auto i = -10; i < 10; ++i)
        {
            for (auto j = -10; j < 10; ++j)
            {
                entities.emplace_back(game::Entity{
                    &mesh,
                    &material,
                    game::Vector3{static_cast<float>(i) * 1.5f, -1.f, static_cast<float>(j) * 1.5f},
                    tex_ptrs,
                    &sampler});
            }
        }

        auto scene = game::Scene{
            .entities = entities |
                        std::views::transform([](const auto &e)
                                              { return &e; }) |
                        std::ranges::to<std::vector>(),
            .ambient = {.r = .2f, .g = .2f, .b = .2f},
            .directional = {.direction = {-1.f, -1.f, -1.f}, .color = {.r = .3f, .g = .3f, .b = .3f}},
            .point = {.position = {5.f, 5.f, 0.f}, .color = {.r = .5f, .g = .5f, .b = .5f}}};

        auto camera = game::Camera{{0.f, 0.f, 5.f},
                                   {0.f, 0.f, 0.f},
                                   {0.f, 1.f, 0.f},
                                   std::numbers::pi_v<float> / 4.f,
                                   800.f,
                                   600.f,
                                   0.001f,
                                   100.f};

        auto key_state = std::unordered_map<game::Key, bool>{};
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
                        }
                        else if constexpr (std::same_as<T, game::MouseEvent>)
                        {
                            static constexpr auto sensitivity = float{0.005f};
                            camera.adjust_pitch(arg.delta_y() * sensitivity);
                            camera.adjust_yaw(arg.delta_x() * sensitivity);
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

            static auto t = 0.f;
            t += 0.05f;
            scene.point.position.x = std::sin(t) * 10.f;
            scene.point.position.z = std::cos(t) * 10.f;

            renderer.render(camera, scene);
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