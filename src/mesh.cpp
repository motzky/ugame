#include "mesh.h"

#include <ranges>
#include <string_view>

#include "auto_release.h"
#include "buffer.h"
#include "buffer_writer.h"
#include "ensure.h"
#include "mesh_loader.h"
#include "opengl.h"
#include "primitives/vertex_data.h"
#include "tlv/tlv_reader.h"

namespace game
{
    Mesh::Mesh(const TlvReader &reader, std::string_view name)
        : _vao{0u, [](auto vao)
               { ::glDeleteVertexArrays(1, &vao); }},
          _vbo{1u},
          _index_count{},
          _index_offset{}
    {
        const auto data = std::ranges::find_if(reader, [name](const auto &e)
                                               { return e.is_mesh(name); });
        ensure(data != std::ranges::end(reader), "failed to load mesh '{}'", name);

        auto mesh = Mesh{(*data).mesh_value()};

        std::ranges::swap(_vao, mesh._vao);
        std::ranges::swap(_vbo, mesh._vbo);
        std::ranges::swap(_index_count, mesh._index_count);
        std::ranges::swap(_index_offset, mesh._index_offset);
    }

    Mesh::Mesh(const MeshData &data)
        : _vao{0u, [](auto vao)
               { ::glDeleteVertexArrays(1, &vao); }},
          _vbo{static_cast<std::uint32_t>(data.vertices.size_bytes() + data.indices.size_bytes())},
          _index_count(static_cast<std::uint32_t>(data.indices.size())),
          _index_offset{data.vertices.size_bytes()}
    {
        {
            auto writer = BufferWriter{_vbo};
            writer.write(data.vertices);
            writer.write(data.indices);
        }

        ::glCreateVertexArrays(1, &_vao);

        ::glVertexArrayVertexBuffer(_vao, 0, _vbo.native_handle(), 0, sizeof(VertexData));
        ::glVertexArrayElementBuffer(_vao, _vbo.native_handle());

        ::glEnableVertexArrayAttrib(_vao, 0); // position
        ::glEnableVertexArrayAttrib(_vao, 1); // normal
        ::glEnableVertexArrayAttrib(_vao, 2); // tangent
        ::glEnableVertexArrayAttrib(_vao, 3); // uv

        ::glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, position));
        ::glVertexArrayAttribFormat(_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, normal));
        ::glVertexArrayAttribFormat(_vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, tangent));
        ::glVertexArrayAttribFormat(_vao, 3, 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, uv));

        ::glVertexArrayAttribBinding(_vao, 0, 0);
        ::glVertexArrayAttribBinding(_vao, 1, 0);
        ::glVertexArrayAttribBinding(_vao, 2, 0);
        ::glVertexArrayAttribBinding(_vao, 3, 0);
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