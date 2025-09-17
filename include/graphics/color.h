#pragma once

#include <format>

namespace game
{
    struct Color
    {
        float r;
        float g;
        float b;
    };

}

template <>
struct std::formatter<game::Color>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::Color &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "r={} g={} b={}",
                              obj.r,
                              obj.g,
                              obj.b);
    }
};