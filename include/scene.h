#pragma once

#include <optional>
#include <vector>

#include "buffer.h"
#include "math/vector3.h"
#include "primitives/color.h"
#include "primitives/debug_lines.h"
#include "primitives/entity.h"
#include "primitives/line_data.h"

namespace game
{
    struct DirectionalLight
    {
        Vector3 direction;
        Color color;
    };

    struct PointLight
    {
        Vector3 position;
        Color color;
        float const_attenuation;
        float linear_attenuation;
        float quad_attenuation;
    };

    struct Scene
    {
        std::vector<const Entity *> entities;
        Color ambient;
        DirectionalLight directional;
        std::vector<PointLight> points;
        std::optional<DebugLines> debug_lines;
    };
}