#pragma once

#include <vector>

#include "color.h"
#include "entity.h"
#include "vector3.h"

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
        PointLight point;
    };
}