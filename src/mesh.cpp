#include "mesh.h"

#include "auto_release.h"
#include "opengl.h"
#include "vertex_data.h"

namespace
{
    // Each face has a single color
    static constexpr game::VertexData vertex_data[] = {
        // Front face (z = +0.5) - Cyan
        {.position = {.x = -0.5f, .y = -0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {.x = 0.5f, .y = -0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = -0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},

        // Back face (z = -0.5) - Magenta
        {.position = {.x = -0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = 0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = 0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},

        // Left face (x = -0.5) - Blue
        {.position = {.x = -0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = -0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {.x = -0.5f, .y = 0.5f, .z = -0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},

        // Right face (x = +0.5) - Green
        {.position = {.x = 0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = -0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = -0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},

        // Top face (y = +0.5) - Red
        {.position = {.x = -0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {.x = -0.5f, .y = 0.5f, .z = 0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = 0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {.x = -0.5f, .y = 0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},

        // Bottom face (y = -0.5) - Yellow
        {.position = {.x = -0.5f, .y = -0.5f, .z = 0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = -0.5f, .z = 0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = -0.5f, .y = -0.5f, .z = 0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = -0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {.x = 0.5f, .y = -0.5f, .z = -0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
    };
}

namespace game
{
    Mesh::Mesh()
        : _vao{0u, [](auto vao)
               { ::glDeleteVertexArrays(1, &vao); }},
          _vbo{0u, [](auto vbo)
               { ::glDeleteBuffers(1, &vbo); }}
    {
        ::glGenVertexArrays(1, &_vao);

        ::glBindVertexArray(_vao);

        ::glGenBuffers(1, &_vbo);

        ::glBindVertexArray(_vao);

        ::glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

        ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(0));
        ::glEnableVertexAttribArray(0);
        ::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        ::glEnableVertexAttribArray(1);

        ::glBindVertexArray(0);
    }

    auto Mesh::bind() const -> void
    {
        ::glBindVertexArray(_vao);
    }

    auto Mesh::unbind() const -> void
    {
        ::glBindVertexArray(0);
    }

}