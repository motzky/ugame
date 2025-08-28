#include <format>
#include <iostream>
#include <print>

#include <utility>

#include "auto_release.h"
#include "error.h"
#include "exception.h"
#include "material.h"
#include "mesh.h"
#include "renderer.h"
#include "shader.h"
#include "window.h"

#include "shader.h"

namespace
{
    static constexpr auto vertex_shader_src = R"(
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 vertex_color;

void main()
{
    gl_Position = vec4(position, 1.0);
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
    std::println("Hello, World!\n");

    try
    {
        game::Window window(800u, 600u); // Create a window with specified width and height

        const auto vertex_shader = game::Shader{vertex_shader_src, game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{fragment_shader_src, game::ShaderType::FRAGMENT};
        auto material = game::Material{vertex_shader, fragment_shader};

        const auto renderer = game::Renderer{std::move(material)};

        while (!window.windowShouldClose())
        {
            renderer.render();
            window.swapBuffers();
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