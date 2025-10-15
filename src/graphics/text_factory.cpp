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

        ensure(::FT_Set_Pixel_Sizes(face.get(), 0, pixel_size) == 0, "failed to set pixel sizes");

        auto tex_desc = TextureDescription{
            .name = std::string(text.data()),
            .format = TextureFormat::R,
            .usage = TextureUsage::SRGB,
            .width{pixel_size * static_cast<std::uint32_t>(text.size())},
            .height{pixel_size},
            .data{}};

        tex_desc.data.resize(tex_desc.height * tex_desc.width, std::byte{});

        auto old_width = 0u;

        for (auto i = 0u; i < text.size(); ++i)
        {
            const auto c = text.data()[i];
            ensure(::FT_Load_Char(face.get(), c, FT_LOAD_RENDER) == 0, "failed to load bitmap for character {}", c);

            auto *glyph = face->glyph;

            const auto bitmap_width = glyph->bitmap.width;
            const auto width_padded = bitmap_width % 4 == 0 ? bitmap_width : bitmap_width + 4 - (bitmap_width % 4);

            const auto y_offset = tex_desc.height - glyph->bitmap_top;

            for (auto row = 0u; row < glyph->bitmap.rows; ++row)
            {
                for (std::uint32_t col = 0; col < width_padded; ++col)
                {
                    auto dst_x = old_width + col + glyph->bitmap_left;
                    auto dst_y = y_offset == 0 ? row + 1 : y_offset + row - 1;
                    auto dst_index = dst_y * tex_desc.width + dst_x;

                    if (col >= bitmap_width)
                    {
                        tex_desc.data[dst_index] = std::byte{};
                        continue;
                    }

                    auto src_index = row * glyph->bitmap.width + col;

                    const auto val = glyph->bitmap.buffer[src_index];
                    tex_desc.data[dst_index] = static_cast<std::byte>(val);
                }
            }

            const auto advance_x = glyph->bitmap.width == 0 ? pixel_size / 2 : glyph->bitmap.width + glyph->bitmap_left;
            old_width += advance_x;
        }

        return {tex_desc, sampler};
    }
}