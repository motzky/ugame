#include <format>
#include <iostream>
#include <print>
#include <variant>

#include <numbers>
#include <utility>

#include "camera.h"
#include "entity.h"
#include "event.h"
#include "exception.h"
#include "key_event.h"
#include "log.h"
#include "material.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"
#include "stop_event.h"
#include "window.h"

#include "shader.h"

namespace
{
    static constexpr auto vertex_shader_src = R"(
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 vertex_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertex_color = color;
}
)";

    static constexpr auto fragment_shader_src = R"(
#version 460 core

in vec3 vertex_color;
out vec4 frag_color;

void main()
{
    frag_color = vec4(vertex_color, 1.0);
}
)";

}

auto main() -> int
{
    game::log::info("hello world");

    try
    {
        game::Window window(800u, 600u); // Create a window with specified width and height

        const auto vertex_shader = game::Shader{vertex_shader_src, game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{fragment_shader_src, game::ShaderType::FRAGMENT};
        auto material = game::Material{vertex_shader, fragment_shader};

        const auto renderer = game::Renderer{};

        const auto mesh = game::Mesh{};
        const auto entity1 = game::Entity{&mesh, &material, {.x = 0.f, .y = -1.f, .z = 0.f}};
        const auto entity2 = game::Entity{&mesh, &material, {.x = 0.f, .y = 1.f, .z = 0.f}};
        auto scene = game::Scene{.entities{&entity1, &entity2}};

        auto camera = game::Camera{{.x = 0.f, .y = 0.f, .z = 5.f},
                                   {.x = 0.f, .y = 0.f, .z = 0.f},
                                   {.x = 0.f, .y = 1.f, .z = 0.f},
                                   std::numbers::pi_v<float> / 4.f,
                                   800.f,
                                   600.f,
                                   0.001f,
                                   100.f};

        auto velocity = game::Vector3{.x = 0.f, .y = 0.f, .z = 0.f};
        auto running = true;

        while (running)
        {
            auto event = window.pump_event();
            while (event && running)
            {
                std::visit([&](auto &&arg)
                           {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr(std::same_as<T, game::StopEvent>)
                    {
                        running = false;
                    }
                    else if constexpr(std::same_as<T, game::KeyEvent>)
                    {                                    
                        game::log::debug("key: {} {}", arg.key(), arg.state());
                        if (arg.key() == game::Key::LEFT_ARROW || arg.key() == game::Key::A)
                        {
                            velocity += arg.state() == game::KeyState::UP 
                                            ? game::Vector3{.x=1.f, .y=0.f, .z=0.f} 
                                            : game::Vector3{.x=-1.f, .y=0.f, .z=0.f};

                        }
                        else if (arg.key() == game::Key::RIGHT_ARROW || arg.key() == game::Key::D)
                        {
                            velocity += arg.state() == game::KeyState::UP 
                                            ? game::Vector3{.x=-1.f, .y=0.f, .z=0.f} 
                                            : game::Vector3{.x=1.f, .y=0.f, .z=0.f};

                        }
                        else if (arg.key() == game::Key::UP_ARROW || arg.key() == game::Key::W)
                        {
                            velocity += arg.state() == game::KeyState::UP 
                                            ? game::Vector3{.x=0.f, .y=0.f, .z=1.f} 
                                            : game::Vector3{.x=0.f, .y=0.f, .z=-1.f};
                        }                              
                        else if (arg.key() == game::Key::DOWN_ARROW || arg.key() == game::Key::S)
                        {
                            velocity += arg.state() == game::KeyState::UP 
                                            ? game::Vector3{.x=0.f, .y=0.f, .z=-1.f} 
                                            : game::Vector3{.x=0.f, .y=0.f, .z=1.f};

                        }
                    } }, *event);
                event = window.pump_event();
            }

            camera.translate(game::Vector3::normalize(velocity));

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