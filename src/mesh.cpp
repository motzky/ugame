#include "mesh.h"

#include <ranges>

#include "auto_release.h"
#include "buffer_writer.h"
#include "buffer.h"
#include "opengl.h"
#include "vertex_data.h"

namespace
{
    // Each face has a single color
    constexpr game::VertexData vertex_data[] = {
        {.position = {-0.5f, -0.5f, 0.5f}, .color = {0.12f, 0.34f, 0.56f}},
        {.position = {0.5f, -0.5f, 0.5f}, .color = {0.78f, 0.23f, 0.89f}},
        {.position = {0.5f, 0.5f, 0.5f}, .color = {0.45f, 0.67f, 0.12f}},
        {.position = {-0.5f, 0.5f, 0.5f}, .color = {0.91f, 0.11f, 0.33f}},
        {.position = {-0.5f, -0.5f, -0.5f}, .color = {0.22f, 0.88f, 0.44f}},
        {.position = {0.5f, -0.5f, -0.5f}, .color = {0.66f, 0.55f, 0.77f}},
        {.position = {0.5f, 0.5f, -0.5f}, .color = {0.39f, 0.99f, 0.21f}},
        {.position = {-0.5f, 0.5f, -0.5f}, .color = {0.81f, 0.14f, 0.62f}},
    };

    constexpr GLuint indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Back face
        5, 4, 7, 7, 6, 5,
        // Left face
        4, 0, 3, 3, 7, 4,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        4, 5, 1, 1, 0, 4};
}

namespace game
{
    Mesh::Mesh()
        : _vao{0u, [](auto vao)
               { ::glDeleteVertexArrays(1, &vao); }},
          _vbo{sizeof(vertex_data) + sizeof(indices)},
          _index_count(static_cast<std::uint32_t>(std::ranges::distance(indices))), _index_offset{sizeof(vertex_data)}
    {
        auto writer = BufferWriter{_vbo};
        writer.write(vertex_data);
        writer.write(indices);

        ::glCreateVertexArrays(1, &_vao);

        ::glVertexArrayVertexBuffer(_vao, 0, _vbo.native_handle(), 0, sizeof(VertexData));
        ::glVertexArrayElementBuffer(_vao, _vbo.native_handle());

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
    auto Mesh::index_count() const -> std::uint32_t
    {
        return _index_count;
    }
    auto Mesh::index_offset() const -> std::uintptr_t
    {
        return _index_offset;
    }

}