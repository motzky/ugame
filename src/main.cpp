#include <format>
#include <iostream>
#include <print>

#include <utility>

#include "auto_release.h"
#include "error.h"
#include "exception.h"
#include "material.h"
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

    static constexpr float vertex_data[] = {
        0.f, .5f, 0.f, 1.f, 0.f, 0.f,
        -.5f, -.5f, 0.f, 0.f, 1.f, 0.f,
        .5f, -.5f, 0.f, 0.f, 0.f, 1.f};

    try
    {
        game::Window window(800u, 600u); // Create a window with specified width and height

        auto vertex_shader = game::Shader{vertex_shader_src, game::ShaderType::VERTEX};
        auto fragment_shader = game::Shader{fragment_shader_src, game::ShaderType::FRAGMENT};

        auto material = game::Material{vertex_shader, fragment_shader};

        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        auto vao = ::GLuint{};
        ::glGenVertexArrays(1, &vao);

        ::glBindVertexArray(vao);

        auto vbo = ::GLuint{};
        ::glGenBuffers(1, &vbo);

        ::glBindVertexArray(vao);

        ::glBindBuffer(GL_ARRAY_BUFFER, vbo);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

        ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(0));
        ::glEnableVertexAttribArray(0);
        ::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        ::glEnableVertexAttribArray(1);

        while (!window.windowShouldClose())
        {
            ::glClear(GL_COLOR_BUFFER_BIT);

            ::glUseProgram(material.native_handle());
            ::glBindVertexArray(vao);
            ::glDrawArrays(GL_TRIANGLES, 0, 3);

            window.swapBuffers();
        }
    }
    catch (const game::Exception &err)
    {
        std::println(std::cerr, "{}\n{}", err.what(), err.stacktrace());
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