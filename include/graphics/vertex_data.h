#pragma once

#include "graphics/color.h"
#include "math/vector3.h"

namespace game
{
    struct UV
    {
        float u;
        float v;
    };

    struct VertexData
    {
        Vector3 position;
        Vector3 normal;
        Vector3 tangent;
        UV uv;
    };
}
