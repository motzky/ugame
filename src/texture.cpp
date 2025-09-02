#include "texture.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ensure.h"
#include "opengl.h"

namespace game
{
    Texture::Texture(std::span<const std::byte> data, std::uint32_t width, std::uint32_t height)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }}
    {
        auto w = static_cast<int>(width);
        auto h = static_cast<int>(height);

        auto num_channels = 3;
        auto raw_data = std::unique_ptr<::stbi_uc, decltype(&::stbi_image_free)>{
            ::stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(data.data()),
                                    static_cast<int>(data.size_bytes()),
                                    &w,
                                    &h,
                                    &num_channels,
                                    0),
            ::stbi_image_free};

        ensure(raw_data, "failed to load texture date");
        ensure(static_cast<std::uint32_t>(w) == width, "width is different {}", w);
        ensure(static_cast<std::uint32_t>(h) == height, "height is different {}", h);
        ensure(num_channels == 4, "num_channels is different: {}", num_channels);

        ::glCreateTextures(GL_TEXTURE_2D, 1u, &_handle);

        ::glTextureStorage2D(_handle, 1, GL_RGBA8, width, height);
        ::glTextureSubImage2D(_handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, raw_data.get());
    }

    auto Texture::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}