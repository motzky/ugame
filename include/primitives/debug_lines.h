#pragma once

#include <cstdint>
#include <span>

#include "auto_release.h"
#include "events/key_event.h"
#include "graphics/buffer.h"
#include "graphics/line_data.h"
#include "graphics/opengl.h"

namespace game
{
    class DebugLines
    {
    public:
        DebugLines(std::span<const LineData> lines);
        auto bind() const -> void;
        auto unbind() const -> void;
        auto count() const -> std::uint32_t;

    private:
        AutoRelease<::GLuint> _vao;
        Buffer _vbo;
        std::uint32_t _count;
    };
}
