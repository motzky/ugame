#include "graphics/cube_map.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <span>
#include <tuple>
#include <vector>

#include "graphics/opengl.h"
#include "log.h"
#include "tlv/tlv_reader.h"
#include "utils/ensure.h"

namespace
{
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
    CubeMap::CubeMap(const TlvReader &reader, std::array<std::string_view, 6> image_names)
        : _handle{0u, [](auto texture)
                  { ::glDeleteTextures(1u, &texture); }}
    {
        const auto datas = image_names | std::views::transform(
                                             [&reader](const auto &e)
                                             {
                                                 const auto data = std::ranges::find_if(reader, [e](const auto &f)
                                                                                        { return f.is_texture(e); });
                                                 ensure(data != std::ranges::end(reader), "failed to load texture");
                                                 return (*data).texture_description_value();
                                             });

        const auto width = datas.front().width;
        const auto height = datas.front().height;

        ensure(
            std::ranges::all_of(
                datas | std::views::transform([](const auto &e)
                                              { return std::make_tuple(e.width, e.height); }),
                [width, height](const auto &e2)
                { return e2 == std::make_tuple(width, height); }),
            "all widths and heights have to be the same");

        ::glCreateTextures(GL_TEXTURE_CUBE_MAP, 1u, &_handle);
        ::glTextureStorage2D(_handle, 1, GL_SRGB8, width, height);

        for (const auto &[index, data] : std::views::enumerate(datas))
        {
            auto num_channels = num_channels_from_format(data.format);
            auto format = GLenum{};

            switch (num_channels)
            {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;

            default:
                ensure(false, "unhandled num_channels {}", num_channels);
                break;
            }
            ::glTextureSubImage3D(_handle, 0, 0, 0, static_cast<::GLuint>(index), data.width, data.height, 1, format, GL_UNSIGNED_BYTE, data.data.data());
        }
    }

    auto CubeMap::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}