#pragma once

#include <format>
#include <string>

namespace game
{
    struct Color
    {
        float r;
        float g;
        float b;
    };

    inline auto to_string(Color obj) -> std::string
    {
        return std::format("r={} g={} b={}", obj.r, obj.g, obj.b);
    }

}
