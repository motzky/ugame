#include "primitives/debug_lines.h"

#include <ranges>
#include <string_view>

#include "buffer_writer.h"
#include "ensure.h"
#include "graphics/line_data.h"
#include "graphics/opengl.h"
#include "mesh_loader.h"
#include "utils/auto_release.h"

namespace game
{
    DebugLines::DebugLines(std::span<const LineData> lines)
        : _vao{0u, [](auto vao)
               { ::glDeleteVertexArrays(1, &vao); }},
          _vbo{static_cast<std::uint32_t>(lines.size_bytes())},
          _count(lines.size())

    {
        {
            auto writer = BufferWriter{_vbo};
            writer.write(lines);
        }

        ::glCreateVertexArrays(1, &_vao);

        ::glVertexArrayVertexBuffer(_vao, 0, _vbo.native_handle(), 0, sizeof(LineData));

        ::glEnableVertexArrayAttrib(_vao, 0); // position
        ::glEnableVertexArrayAttrib(_vao, 1); // color

        ::glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(LineData, position));
        ::glVertexArrayAttribFormat(_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(LineData, color));

        ::glVertexArrayAttribBinding(_vao, 0, 0);
        ::glVertexArrayAttribBinding(_vao, 1, 0);
    }

    auto DebugLines::bind() const -> void
    {
        ::glBindVertexArray(_vao);
    }

    auto DebugLines::unbind() const -> void
    {
        ::glBindVertexArray(0);
    }

    auto DebugLines::count() const -> std::uint32_t
    {
        return _count;
    }
}