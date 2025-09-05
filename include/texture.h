#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "auto_release.h"
#include "opengl.h"

namespace game
{
    enum class TextureUsage
    {
        FRAMEBUFFER,
        DEPTH,
    };

    class Texture
    {
    public:
        Texture(TextureUsage, std::uint32_t width, std::uint32_t height);
        Texture(std::span<const std::byte> data, std::uint32_t width, std::uint32_t height);

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<GLuint> _handle;
    };

}