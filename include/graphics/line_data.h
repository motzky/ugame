#pragma once

#include <format>
#include <string>

#include "graphics/color.h"
#include "math/vector3.h"

namespace game
{
    struct LineData
    {
        Vector3 position;
        Color color;
    };

    inline auto to_string(const LineData &obj) -> std::string
    {
        return std::format("p={} c={}", obj.position, obj.color);
    }
}
