#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "auto_release.h"
#include "opengl.h"

namespace game
{
    class CubeMap
    {
    public:
        CubeMap(const std::vector<std::span<const std::byte>> &faces, std::uint32_t width, std::uint32_t height);

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<GLuint> _handle;
    };
}
