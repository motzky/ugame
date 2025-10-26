#include "graphics/texture.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <string>

#include "graphics/opengl.h"
#include "log.h"
#include "tlv/tlv_reader.h"
#include "utils/ensure.h"
#include "utils/formatter.h"

namespace
{
    auto get_storage_format(game::TextureUsage usage, std::int32_t num_channels) -> ::GLenum
    {
        switch (num_channels)
        {
            using enum game::TextureUsage;
        case 1:
            return GL_R8;
        case 3:
            switch (usage)
            {
            case SRGB:
                return GL_SRGB8;
            case DATA:
                return GL_RGB8;
            default:
                return GL_RGB8;
            }
        case 4:
            switch (usage)
            {
            case SRGB:
                return GL_SRGB8_ALPHA8;
            case DATA:
                return GL_RGBA8;
            default:
                return GL_RGBA8;
            }

        default:
            game::ensure(false, "unhandled num_channels {}", num_channels);
            return 0u;
        }
    }

    auto get_sub_image_format(std::int32_t num_channels) -> ::GLenum
    {
        switch (num_channels)
        {
        case 1:
            return GL_RED;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            game::ensure(false, "unhandled num_channels {}", num_channels);
            return 0u;
        }
    }

    auto num_channels_from_format(game::TextureFormat format) -> std::int32_t
    {
        switch (format)
        {
            using enum game::TextureFormat;
        case R:
            return 1;
        case RGB:
            return 3;
        case RGBA:
            return 4;
        default:
            game::ensure(false, "unhandled format {}", format);
            return 0u;
        }
    }
}

namespace game
{
    Texture::Texture(const TextureDescription &data, const TextureSampler *sampler)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }},
          _sampler(sampler),
          _width{data.width},
          _height{data.height}
    {
        log::info("creating texture with: {}", data);

        ::glCreateTextures(GL_TEXTURE_2D, 1u, &_handle);

        auto num_channels = num_channels_from_format(data.format);

        ::GLsizei levels = 1;
        if (data.usage == TextureUsage::SRGB)
        {
            levels += static_cast<::GLsizei>(std::floor(std::log(std::max(data.width, data.height))));
        }

        ::glTextureStorage2D(
            _handle,
            levels,
            get_storage_format(data.usage, num_channels),
            data.width,
            data.height);
        ::glTextureSubImage2D(_handle, 0, 0, 0, data.width, data.height, get_sub_image_format(num_channels), GL_UNSIGNED_BYTE, data.data.data());
        if (data.usage == TextureUsage::SRGB)
        {
            ::glGenerateTextureMipmap(_handle);
        }
    }

    Texture::Texture(const TlvReader &reader, std::string_view name, const TextureSampler *sampler)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }},
          _sampler(sampler)

    {
        const auto data = std::ranges::find_if(reader, [name](const auto &e)
                                               { return e.is_texture(name); });
        ensure(data != std::ranges::end(reader), "failed to load texture '{}'", name);

        auto tex = Texture{(*data).texture_description_value(), _sampler};

        std::ranges::swap(_handle, tex._handle);
    }

    Texture::Texture(TextureUsage usage, std::uint32_t width, std::uint32_t height)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }},
          _sampler(nullptr),
          _width{width},
          _height{height}
    {
        ::glCreateTextures(GL_TEXTURE_2D, 1u, &_handle);
        switch (usage)
        {
            using enum TextureUsage;
        case FRAMEBUFFER:
            ::glTextureStorage2D(_handle, 1, GL_RGB16F, width, height);
            break;
        case DEPTH:
            ::glTextureStorage2D(_handle, 1, GL_DEPTH_COMPONENT24, width, height);
            break;
        default:
            ensure(false, "invalid usage");
            break;
        }
    }

    auto Texture::native_handle() const -> ::GLuint
    {
        return _handle;
    }
    auto Texture::sampler() const -> const TextureSampler *
    {
        return _sampler;
    }

    auto Texture::width() const -> std::uint32_t
    {
        return _width;
    }
    auto Texture::height() const -> std::uint32_t
    {
        return _height;
    }

    auto to_string(TextureUsage obj) -> std::string
    {
        switch (obj)
        {
            using enum game::TextureUsage;
        case FRAMEBUFFER:
            return "FRAMEBUFFER";
        case DEPTH:
            return "DEPTH";
        case SRGB:
            return "SRGB";
        case DATA:
            return "DATA";
        default:
            return std::format("{}", std::to_underlying(obj));
        }
    }

    auto to_string(TextureFormat obj) -> std::string
    {
        switch (obj)
        {
            using enum game::TextureFormat;
        case R:
            return "R";
        case RGB:
            return "RGB";
        case RGBA:
            return "RGBA";
        default:
            return std::format("{}", std::to_underlying(obj));
        }
    }

    auto to_string(const TextureDescription &obj) -> std::string
    {
        return std::format("width={} height={} format={} usage={} data={}",
                           obj.width,
                           obj.height,
                           obj.format,
                           obj.usage,
                           obj.data.size());
    }

}