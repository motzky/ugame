#pragma once

#include <vector>

#include "math_primitives/vector3.h"
#include "primitives/color.h"
#include "primitives/entity.h"

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
    };
}