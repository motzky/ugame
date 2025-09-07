#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <vector>

#include "auto_release.h"
#include "opengl.h"

namespace game
{
    enum class TextureUsage
    {
        FRAMEBUFFER,
        DEPTH,
        SRGB,
        DATA
    };

    enum class TextureFormat
    {
        RGB,
        RGBA
    };

    struct TextureData
    {
        std::string name;
        TextureFormat format;
        TextureUsage usage;
        std::uint32_t width;
        std::uint32_t height;
        std::vector<std::byte> data;
    };

    class Texture
    {
    public:
        Texture(TextureUsage usage, std::uint32_t width, std::uint32_t height);
        Texture(const TextureData &data);
        Texture(TextureUsage usage, std::span<const std::byte> data, std::uint32_t width, std::uint32_t height);

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<GLuint> _handle;
    };

}

template <>
struct std::formatter<game::TextureData>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::TextureData &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "width={} height={} format={} usage={} data={}",
                              obj.width,
                              obj.height,
                              std::to_underlying(obj.format),
                              std::to_underlying(obj.usage),
                              obj.data.size());
    }
};
