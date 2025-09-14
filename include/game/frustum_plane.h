#pragma once

#include "math/vector3.h"

namespace game
{
    struct FrustumPlane
    {
        Vector3 normal;
        float distance;
    };
}