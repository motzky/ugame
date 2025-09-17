#pragma once

#include <format>

#include "graphics/color.h"
#include "math/vector3.h"

namespace game
{
    struct LineData
    {
        Vector3 position;
        Color color;
    };
}

template <>
struct std::formatter<game::LineData>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::LineData &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "p={} c={}",
                              obj.position,
                              obj.color);
    }
};