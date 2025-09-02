#include "mesh.h"

#include <ranges>

#include "auto_release.h"
#include "buffer_writer.h"
#include "buffer.h"
#include "opengl.h"
#include "vertex_data.h"

namespace
{
    // 6 faces × 4 vertices per face = 24 vertices
    constexpr game::VertexData vertex_data[] = {
        // Front face
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
        // Right face
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
        // Back face
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},
        // Left face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},
        // Top face
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},
        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
    };

    constexpr GLuint indices[] = {
        // Each face: 2 triangles (6 indices)
        // Front face
        0, 1, 2, 2, 3, 0,
        // Right face
        4, 5, 6, 6, 7, 4,
        // Back face
        8, 9, 10, 10, 11, 8,
        // Left face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20};
}

namespace game
{
    Mesh::Mesh()
        : _vao{0u, [](auto vao)
               { ::glDeleteVertexArrays(1, &vao); }},
          _vbo{sizeof(vertex_data) + sizeof(indices)},
          _index_count(static_cast<std::uint32_t>(std::ranges::distance(indices))), _index_offset{sizeof(vertex_data)}
    {
        {
            auto writer = BufferWriter{_vbo};
            writer.write(vertex_data);
            writer.write(indices);
        }

        ::glCreateVertexArrays(1, &_vao);

        ::glVertexArrayVertexBuffer(_vao, 0, _vbo.native_handle(), 0, sizeof(VertexData));
        ::glVertexArrayElementBuffer(_vao, _vbo.native_handle());

        ::glEnableVertexArrayAttrib(_vao, 0);
        ::glEnableVertexArrayAttrib(_vao, 1);

        ::glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, position));
        // ::glVertexArrayAttribFormat(_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, color));
        ::glVertexArrayAttribFormat(_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, uv));

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
    auto Mesh::index_count() const -> std::uint32_t
    {
        return _index_count;
    }
    auto Mesh::index_offset() const -> std::uintptr_t
    {
        return _index_offset;
    }

}