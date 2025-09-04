#pragma once

#include "vector3.h"
#include "color.h"

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
