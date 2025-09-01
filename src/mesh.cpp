#include "mesh.h"

#include "auto_release.h"
#include "opengl.h"
#include "vertex_data.h"

namespace
{
    // Each face has a single color
    static constexpr game::VertexData vertex_data[] = {
        // Front face (z = +0.5) - Cyan
        {.position = {-0.5f, -0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {0.5f, -0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {0.5f, 0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {-0.5f, -0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {0.5f, 0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},
        {.position = {-0.5f, 0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 1.f}},

        // Back face (z = -0.5) - Magenta
        {.position = {-0.5f, -0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {0.5f, 0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {0.5f, -0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {-0.5f, -0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {-0.5f, 0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},
        {.position = {0.5f, 0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 1.f}},

        // Left face (x = -0.5) - Blue
        {.position = {-0.5f, -0.5f, -0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {-0.5f, -0.5f, 0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {-0.5f, 0.5f, 0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {-0.5f, -0.5f, -0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {-0.5f, 0.5f, 0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},
        {.position = {-0.5f, 0.5f, -0.5f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}},

        // Right face (x = +0.5) - Green
        {.position = {0.5f, -0.5f, -0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {0.5f, 0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {0.5f, -0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {0.5f, -0.5f, -0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {0.5f, 0.5f, -0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},
        {.position = {0.5f, 0.5f, 0.5f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}},

        // Top face (y = +0.5) - Red
        {.position = {-0.5f, 0.5f, 0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {0.5f, 0.5f, 0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {0.5f, 0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {-0.5f, 0.5f, 0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {0.5f, 0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},
        {.position = {-0.5f, 0.5f, -0.5f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}},

        // Bottom face (y = -0.5) - Yellow
        {.position = {-0.5f, -0.5f, 0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {0.5f, -0.5f, -0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {0.5f, -0.5f, 0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {-0.5f, -0.5f, 0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {-0.5f, -0.5f, -0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
        {.position = {0.5f, -0.5f, -0.5f}, .color = {.r = 1.f, .g = 1.f, .b = 0.f}},
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
        ::glCreateBuffers(1, &_vbo);
        ::glNamedBufferStorage(_vbo, sizeof(vertex_data), vertex_data, GL_DYNAMIC_STORAGE_BIT);

        ::glCreateVertexArrays(1, &_vao);

        ::glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(VertexData));

        ::glEnableVertexArrayAttrib(_vao, 0);
        ::glEnableVertexArrayAttrib(_vao, 1);

        ::glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, position));
        ::glVertexArrayAttribFormat(_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, color));

        ::glVertexArrayAttribBinding(_vao, 0, 0);
        ::glVertexArrayAttribBinding(_vao, 1, 0);
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