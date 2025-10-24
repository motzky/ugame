#include "graphics/text_factory.h"

#include <memory>
#include <numeric>
#include <ranges>
#include <string>
#include <string_view>

#include <ft2build.h>

#include <freetype/freetype.h>

#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "log.h"
#include "resources/resource_loader.h"
#include "utils/ensure.h"

namespace
{
    auto text_widen(std::string_view text) -> std::wstring
    {
        return std::wstring(text.begin(), text.end());
    }

    auto ft_pos_to_int(::FT_Pos pos) -> std::int32_t
    {
        game::ensure((pos & 63) == 0, "failed to convert 26.6 fixed point value to int: fration not 0: {}", pos);

        return static_cast<std::int32_t>(pos >> 6);
    }
    auto ft_pos_to_uint(::FT_Pos pos) -> std::uint32_t
    {
        game::ensure((pos & 63) == 0, "failed to convert 26.6 fixed point value to uint: fration not 0: {}", pos);
        auto integer_part = pos >> 6;
        game::ensure(integer_part >= 0, "failed to convert 26.6 fixed point value to uint: must be positive {}", pos);

        return static_cast<std::uint32_t>(integer_part);
    }
}

namespace game
{
    TextFactory::TextFactory(const ResourceLoader &resource_loader)
        : _ft{}, _resource_loader{resource_loader}
    {
        ensure(::FT_Init_FreeType(std::addressof(_ft)) == 0, "failed to init freetype lib");
    }
    TextFactory::~TextFactory()
    {
        ::FT_Done_FreeType(_ft);
    }

    auto TextFactory::create(std::string_view text, const TextureSampler *sampler, const std::uint32_t pixel_size) const -> Texture
    {
        log::info("creating text for: {}", text);

        const auto releaseFace = [](auto *up)
        { ::FT_Done_Face(up); };

        auto face = std::unique_ptr<FT_FaceRec_, decltype(releaseFace)>{};

        // const auto font_data = _resource_loader.load("CaskaydiaCoveNerdFont-Regular.ttf").as_bytes();

        ensure(::FT_New_Face(_ft, "./assets/CaskaydiaCoveNerdFont-Regular.ttf", 0, std::out_ptr(face)) == 0, "failed to load font");

        ensure(::FT_Select_Charmap(face.get(), FT_ENCODING_UNICODE) == 0, "failed to select unicode char map");

        ensure(::FT_Set_Pixel_Sizes(face.get(), 0, pixel_size) == 0, "failed to set pixel sizes");

        const auto wide_text = text_widen(text);

        auto baseline_y = 0;
        auto y_min = 0;
        auto bearing_y_max = 0;
        auto x_max = 0u;
        auto height_max = 0;

        for (auto i = 0u; i < wide_text.size(); ++i)
        {
            const auto c = wide_text.data()[i];
            ensure(::FT_Load_Char(face.get(), c, FT_LOAD_RENDER) == 0, "failed to load bitmap for character {}", std::to_string(c));

            auto *glyph = face->glyph;

            const auto glyph_baseline_y = glyph->bitmap_top;
            if (glyph_baseline_y > baseline_y)
            {
                baseline_y = glyph_baseline_y;
            }
            const auto glyph_height = ft_pos_to_int(glyph->metrics.height);
            if (glyph_height > height_max)
            {
                height_max = glyph_height;
            }
            const auto glyph_bearing_y = ft_pos_to_int(glyph->metrics.horiBearingY);
            if (glyph_bearing_y > bearing_y_max)
            {
                bearing_y_max = glyph_bearing_y;
            }
            const auto glyph_y_min = glyph_height - glyph_bearing_y;
            if (glyph_y_min > y_min)
            {
                y_min = glyph_y_min;
            }

            const auto glyph_advance = ft_pos_to_int(glyph->metrics.horiAdvance);

            x_max += glyph_advance;
        }

        // align to 4 bytes
        x_max = x_max % 4 == 0 ? x_max : x_max + 4 - (x_max % 4);
        auto h = static_cast<std::uint32_t>(height_max) + static_cast<std::uint32_t>(y_min);
        h = h % 4 == 0 ? h : h + 4 - (h % 4);

        auto tex_desc = TextureDescription{
            .name = std::string(text.data()),
            .format = TextureFormat::R,
            .usage = TextureUsage::SRGB,
            .width{x_max},
            .height{h},
            .data{}};

        tex_desc.data.resize(tex_desc.height * tex_desc.width, std::byte{});

        auto pen = 0u;

        for (auto i = 0u; i < wide_text.size(); ++i)
        {
            const auto c = wide_text.data()[i];
            ensure(::FT_Load_Char(face.get(), c, FT_LOAD_RENDER) == 0, "failed to load bitmap for character {}", std::to_string(c));

            auto *glyph = face->glyph;

            const auto glyph_width = ft_pos_to_uint(glyph->metrics.width);
            const auto glyph_advance = ft_pos_to_int(glyph->metrics.horiAdvance);
            const auto glyph_bearing_x = ft_pos_to_int(glyph->metrics.horiBearingX);
            const auto glyph_bearing_y = ft_pos_to_int(glyph->metrics.horiBearingY);

            for (auto row = 0u; row < glyph->bitmap.rows; ++row)
            {
                const auto dst_y = baseline_y - glyph_bearing_y + row;

                for (std::uint32_t col = 0u; col < glyph_width; ++col)
                {
                    const auto dst_x = pen + glyph_bearing_x + col;

                    const auto dst_index = dst_y * tex_desc.width + dst_x;
                    const auto src_index = row * glyph->bitmap.width + col;

                    const auto val = glyph->bitmap.buffer[src_index];
                    tex_desc.data[dst_index] = static_cast<std::byte>(val);
                }
            }

            pen += glyph_advance;
        }

        return {tex_desc, sampler};
    }
}