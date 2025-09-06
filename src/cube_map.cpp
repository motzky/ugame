#include "cube_map.h"

#include <cstddef>
#include <cstdint>
#include <ranges>
#include <span>
#include <vector>

#include <stb_image.h>

#include "ensure.h"
#include "log.h"
#include "opengl.h"

namespace game
{
    CubeMap::CubeMap(const std::vector<std::span<const std::byte>> &faces, std::uint32_t width, std::uint32_t height)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }}
    {
        ::glCreateTextures(GL_TEXTURE_CUBE_MAP, 1u, &_handle);
        ::glTextureStorage2D(_handle, 1, GL_SRGB8, width, height);

        for (const auto &[index, face] : std::views::enumerate(faces))
        {
            auto w = int{0};
            auto h = int{0};
            auto num_channels = int{0};

            auto raw_data = std::unique_ptr<::stbi_uc, decltype(&::stbi_image_free)>{
                ::stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(face.data()),
                                        static_cast<int>(face.size_bytes()),
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

            switch (num_channels)
            {
            case 1:
                // ::glTextureStorage3D(_handle, 1, GL_R8, w, h);
                ::glTextureSubImage3D(_handle, 0, 0, 0, static_cast<::GLuint>(index), w, h, 1, GL_RED, GL_UNSIGNED_BYTE, raw_data.get());
                break;
            case 3:
                // ::glTextureStorage3D(_handle, 1, GL_RGB8, w, h);
                ::glTextureSubImage3D(_handle, 0, 0, 0, static_cast<::GLuint>(index), w, h, 1, GL_RGB, GL_UNSIGNED_BYTE, raw_data.get());
                break;
            case 4:
                // ::glTextureStorage3D(_handle, 1, GL_RGBA8, w, h);
                ::glTextureSubImage3D(_handle, 0, 0, 0, static_cast<::GLuint>(index), w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, raw_data.get());
                break;

            default:
                ensure(false, "unhandled num_channels {}", num_channels);
                break;
            }
        }
    }

    auto CubeMap::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}