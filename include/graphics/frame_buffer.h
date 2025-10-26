#pragma once

#include <cstdint>

#include "opengl.h"
#include "texture.h"
#include "utils/auto_release.h"

namespace game
{
    class FrameBuffer
    {
    public:
        FrameBuffer(std::uint32_t width, std::uint32_t height, std::uint8_t samples = 1);

        auto native_handle() const -> ::GLuint;
        auto bind() const -> void;
        auto unbind() const -> void;

        auto width() const -> std::uint32_t;
        auto height() const -> std::uint32_t;

        auto color_texture() const -> const Texture &;

    private:
        AutoRelease<::GLuint> _handle;
        std::uint32_t _width;
        std::uint32_t _height;
        Texture _color_texture;
        Texture _depth_texture;
    };
}