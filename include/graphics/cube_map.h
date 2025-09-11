#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "auto_release.h"
#include "graphics/opengl.h"

namespace game
{
    class TlvReader;

    class CubeMap
    {
    public:
        CubeMap(const std::vector<std::span<const std::byte>> &faces, std::uint32_t width, std::uint32_t height);
        CubeMap(const TlvReader &reader, std::array<std::string_view, 6> image_names);

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<GLuint> _handle;
    };
}
