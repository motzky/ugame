#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "graphics/opengl.h"
#include "utils/auto_release.h"

namespace game
{
    class TextureSampler;
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
        Texture(const TextureDescription &data, const TextureSampler *sampler);
        Texture(const TlvReader &reader, std::string_view name, const TextureSampler *sampler);
        // Texture(TextureUsage usage, std::span<const std::byte> data, std::uint32_t width, std::uint32_t height, const TextureSampler *sampler);

        auto native_handle() const -> ::GLuint;
        auto sampler() const -> const TextureSampler *;

    private:
        AutoRelease<GLuint> _handle;
        const TextureSampler *_sampler;
    };

    auto to_string(TextureUsage obj) -> std::string;
    auto to_string(TextureFormat obj) -> std::string;
    auto to_string(const TextureDescription &obj) -> std::string;
}
