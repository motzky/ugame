#pragma once

#include <cstdint>

#include "auto_release.h"
#include "opengl.h"
#include "texture.h"

namespace game
{
    class FrameBuffer
    {
    public:
        FrameBuffer(std::uint32_t width, std::uint32_t height);

        auto native_handle() -> ::GLuint;
        auto bind() -> void;
        auto unbind() -> void;

        auto width() -> std::uint32_t;
        auto height() -> std::uint32_t;

    private:
        AutoRelease<::GLuint> _handle;
        std::uint32_t _width;
        std::uint32_t _height;
        Texture _color_texture;
        Texture _depth_texture;
    };
}