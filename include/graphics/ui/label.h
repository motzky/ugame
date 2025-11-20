#pragma once

#include <cstdint>

namespace game
{

    class Texture;
    struct Color;

    namespace graphics::ui
    {
        struct Label
        {
            std::uint32_t x;
            std::uint32_t y;
            const Texture *texture;
            Color color;
        };
    }

}