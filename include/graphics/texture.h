#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "opengl.h"
#include "utils/auto_release.h"

namespace game
{
    class TlvReader;

    enum class TextureUsage
    {
        FRAMEBUFFER,
        DEPTH,
        SRGB,
        DATA
    };

    enum class TextureFormat
    {
        R,
        RGB,
        RGBA
    };

    struct TextureDescription
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
        Texture(const TextureDescription &data);
        Texture(const TlvReader &reader, std::string_view name);
        // Texture(TextureUsage usage, std::span<const std::byte> data, std::uint32_t width, std::uint32_t height);

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<GLuint> _handle;
    };

}

template <>
struct std::formatter<game::TextureUsage>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::TextureUsage &obj, std::format_context &ctx) const
    {
        switch (obj)
        {
            using enum game::TextureUsage;
        case FRAMEBUFFER:
            return std::format_to(ctx.out(), "FRAMEBUFFER");
        case DEPTH:
            return std::format_to(ctx.out(), "DEPTH");
        case SRGB:
            return std::format_to(ctx.out(), "SRGB");
        case DATA:
            return std::format_to(ctx.out(), "DATA");
        default:
            return std::format_to(ctx.out(), "{}", std::to_underlying(obj));
        }
    }
};

template <>
struct std::formatter<game::TextureFormat>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::TextureFormat &obj, std::format_context &ctx) const
    {
        switch (obj)
        {
            using enum game::TextureFormat;
        case R:
            return std::format_to(ctx.out(), "R");
        case RGB:
            return std::format_to(ctx.out(), "RGB");
        case RGBA:
            return std::format_to(ctx.out(), "RGBA");
        default:
            return std::format_to(ctx.out(), "{}", std::to_underlying(obj));
        }
    }
};

template <>
struct std::formatter<game::TextureDescription>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::TextureDescription &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "width={} height={} format={} usage={} data={}",
                              obj.width,
                              obj.height,
                              obj.format,
                              obj.usage,
                              obj.data.size());
    }
};
