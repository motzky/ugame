#include "graphics/texture.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ranges>
#include <span>

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

#include "ensure.h"
#include "graphics/opengl.h"
#include "log.h"
#include "tlv/tlv_reader.h"

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
    Texture::Texture(const TextureDescription &data)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }}
    {
        log::info("creating texture with: {}", data);

        ::glCreateTextures(GL_TEXTURE_2D, 1u, &_handle);

        auto num_channels = num_channels_from_format(data.format);

        ::glTextureStorage2D(_handle, 1, get_storage_format(data.usage, num_channels), data.width, data.height);
        ::glTextureSubImage2D(_handle, 0, 0, 0, data.width, data.height, get_sub_image_format(num_channels), GL_UNSIGNED_BYTE, data.data.data());
    }

    Texture::Texture(const TlvReader &reader, std::string_view name)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }}
    {
        const auto data = std::ranges::find_if(reader, [name](const auto &e)
                                               { return e.is_texture(name); });
        ensure(data != std::ranges::end(reader), "failed to load texture '{}'", name);

        auto tex = Texture{(*data).texture_description_value()};

        std::ranges::swap(_handle, tex._handle);
    }

    Texture::Texture(TextureUsage usage, std::uint32_t width, std::uint32_t height)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }}
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

    // Texture::Texture(TextureUsage usage, std::span<const std::byte> data, std::uint32_t /*width*/, std::uint32_t /*height*/)
    //     : _handle{0u, [](auto texture)
    //               { ::glDeleteTextures(1u, &texture); }}
    // {
    //     TextureUsage valid_usage[] = {TextureUsage::SRGB, TextureUsage::DATA};
    //     ensure(std::ranges::contains(valid_usage, usage), "invalid usage");

    //     auto w = int{0};
    //     auto h = int{0};
    //     auto num_channels = int{0};

    //     auto raw_data = std::unique_ptr<::stbi_uc, decltype(&::stbi_image_free)>{
    //         ::stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(data.data()),
    //                                 static_cast<int>(data.size_bytes()),
    //                                 &w,
    //                                 &h,
    //                                 &num_channels,
    //                                 0),
    //         ::stbi_image_free};

    //     ensure(raw_data, "failed to load texture date");

    //     ::glCreateTextures(GL_TEXTURE_2D, 1u, &_handle);

    //     ::glTextureStorage2D(_handle, 1, get_storage_format(usage, num_channels), w, h);
    //     ::glTextureSubImage2D(_handle, 0, 0, 0, w, h, get_sub_image_format(num_channels), GL_UNSIGNED_BYTE, raw_data.get());
    // }

    auto Texture::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}