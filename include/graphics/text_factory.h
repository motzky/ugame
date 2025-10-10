#pragma once

#include <string_view>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "resources/resource_loader.h"
#include "utils/auto_release.h"

namespace game
{
    class TextFactory
    {
    public:
        TextFactory(const ResourceLoader &resource_loader);
        ~TextFactory();
        auto create(std::string_view text, const TextureSampler *sampler) const -> Texture;

    private:
        FT_Library _ft;
        const ResourceLoader &_resource_loader;
    };
}
