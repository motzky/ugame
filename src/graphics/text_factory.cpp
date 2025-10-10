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

    auto TextFactory::create(std::string_view text, const TextureSampler *sampler) const -> Texture
    {
        log::debug("creating text for: {}", text);

        const auto releaseFace = [](auto *up)
        { ::FT_Done_Face(up); };

        auto face = std::unique_ptr<FT_FaceRec_, decltype(releaseFace)>{};

        const auto font_data = _resource_loader.load("CaskaydiaCoveNerdFont-Regular.ttf").as_bytes();

        // ensure(::FT_New_Face(_ft, "CaskaydiaCoveNerdFont-Regular.ttf", 0, std::out_ptr(face)) == 0, "failed to load font");
        ensure(::FT_New_Memory_Face(_ft, reinterpret_cast<const unsigned char *>(font_data.data()), font_data.size_bytes(), 0, std::out_ptr(face)) == 0, "failed to load font");

        // set pixel size to 0 x 48 -> 48x48
        ::FT_Set_Pixel_Sizes(face.get(), 0, 48);

        auto tex_desc = TextureDescription{
            .name = std::string(text.data()),
            .format = TextureFormat::RGB,
            .usage = TextureUsage::SRGB,
            .width{},
            .height{},
            .data{}};

        for (auto i = 0u; i < text.size(); ++i)
        {
            const auto c = text.data()[i];
            const auto index = ::FT_Get_Char_Index(face.get(), c);

            ensure(::FT_Load_Char(face.get(), index, FT_LOAD_RENDER) == 0, "failed to load bitmap for character {}", c);

            auto *glyph = face->glyph;
            tex_desc.width += glyph->bitmap.width;
            if (glyph->bitmap.rows > tex_desc.height)
            {
                tex_desc.height = glyph->bitmap.rows;
            }

            // const auto buffer_size = glyph->bitmap.width * glyph->bitmap.rows;
            // const auto stride = glyph->bitmap.pitch;
        }

        return {tex_desc, sampler};
    }
}