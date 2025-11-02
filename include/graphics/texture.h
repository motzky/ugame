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
        Texture(TextureUsage usage, std::uint32_t width, std::uint32_t height, std::uint8_t samples = 1);
        Texture(const TextureDescription &data, const TextureSampler *sampler);
        Texture(const TlvReader &reader, std::string_view name, const TextureSampler *sampler);

        Texture(Texture &&) noexcept = default;
        auto operator=(Texture &&) noexcept -> Texture & = default;

        auto native_handle() const -> ::GLuint;
        auto sampler() const -> const TextureSampler *;
        auto width() const -> std::uint32_t;
        auto height() const -> std::uint32_t;

    private:
        AutoRelease<::GLuint> _handle;
        const TextureSampler *_sampler;
        std::uint32_t _width;
        std::uint32_t _height;
    };

    auto to_string(TextureUsage obj) -> std::string;
    auto to_string(TextureFormat obj) -> std::string;
    auto to_string(const TextureDescription &obj) -> std::string;
}
