#include "texture.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ensure.h"
#include "log.h"
#include "opengl.h"

namespace
{
    auto get_storage_format(std::int32_t num_channels) -> ::GLenum
    {
        switch (num_channels)
        {
        case 1:
            return GL_R8;
            break;
        case 3:
            return GL_RGB8;
            break;
        case 4:
            return GL_RGBA8;
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
            break;
        case 3:
            return GL_RGB;
            break;
        case 4:
            return GL_RGBA;
        default:
            game::ensure(false, "unhandled num_channels {}", num_channels);
            return 0u;
        }
    }
}

namespace game
{
    Texture::Texture(TextureUsage usage, std::uint32_t width, std::uint32_t height)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }}
    {
        ::glCreateTextures(GL_TEXTURE_2D, 1u, &_handle);
        switch (usage)
        {
            using enum TextureUsage;
        case FRAMEBUFFER:
            ::glTextureStorage2D(_handle, 1, get_storage_format(3), width, height);
            break;
        case DEPTH:
            ::glTextureStorage2D(_handle, 1, GL_DEPTH_COMPONENT24, width, height);
            break;
        }
    }

    Texture::Texture(std::span<const std::byte> data, std::uint32_t /*width*/, std::uint32_t /*height*/)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }}
    {
        auto w = int{0};
        auto h = int{0};
        auto num_channels = int{0};

        auto raw_data = std::unique_ptr<::stbi_uc, decltype(&::stbi_image_free)>{
            ::stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(data.data()),
                                    static_cast<int>(data.size_bytes()),
                                    &w,
                                    &h,
                                    &num_channels,
                                    0),
            ::stbi_image_free};

        ensure(raw_data, "failed to load texture date");

        log::info("loaded image width {}", w);
        log::info("loaded image height {}", h);
        log::info("loaded image channels {}", num_channels);

        ::glCreateTextures(GL_TEXTURE_2D, 1u, &_handle);

        ::glTextureStorage2D(_handle, 1, get_storage_format(num_channels), w, h);
        ::glTextureSubImage2D(_handle, 0, 0, 0, w, h, get_sub_image_format(num_channels), GL_UNSIGNED_BYTE, raw_data.get());
    }

    auto Texture::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}