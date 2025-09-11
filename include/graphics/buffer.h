#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "opengl.h"
#include "utils/auto_release.h"

namespace game
{
    class Buffer
    {
    public:
        Buffer(std::uint32_t size);

        auto write(std::span<const std::byte>, std::size_t offset) const -> void;

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<::GLuint> _buffer;
        std::uint32_t _size;
    };
}