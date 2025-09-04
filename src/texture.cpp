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

namespace game
{
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
        // ensure(static_cast<std::uint32_t>(w) == width, "width is different {}", w);
        // ensure(static_cast<std::uint32_t>(h) == height, "height is different {}", h);
        // ensure(num_channels == 4, "num_channels is different: {}", num_channels);
        log::info("loaded image width {}", w);
        log::info("loaded image height {}", h);
        log::info("loaded image channels {}", num_channels);

        ::glCreateTextures(GL_TEXTURE_2D, 1u, &_handle);

        switch (num_channels)
        {
        case 1:
            ::glTextureStorage2D(_handle, 1, GL_R8, w, h);
            ::glTextureSubImage2D(_handle, 0, 0, 0, w, h, GL_RED, GL_UNSIGNED_BYTE, raw_data.get());
            break;
        case 3:
            ::glTextureStorage2D(_handle, 1, GL_RGB8, w, h);
            ::glTextureSubImage2D(_handle, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, raw_data.get());
            break;
        case 4:
            ::glTextureStorage2D(_handle, 1, GL_RGBA8, w, h);
            ::glTextureSubImage2D(_handle, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, raw_data.get());
            break;

        default:
            ensure(false, "unhandled num_channels {}", num_channels);
            break;
        }
    }

    auto Texture::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}