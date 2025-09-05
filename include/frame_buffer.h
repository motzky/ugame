#pragma once

#include <cstdint>

#include "opengl.h"
#include "auto_release.h"

namespace game
{
    class FrameBuffer
    {
    public:
        FrameBuffer(std::uint32_t width, std::uint32_t height);

        auto native_handle() -> ::GLuint;
        auto bind() -> void;
        auto unbind() -> void;

    private:
        AutoRelease<::GLuint> _handle;
    };
}