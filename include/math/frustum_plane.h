#pragma once

#include "math/matrix3.h"
#include "math/vector3.h"

namespace game
{
    struct FrustumPlane
    {
        FrustumPlane() = default;

        FrustumPlane(const game::Vector3 &p1, const Vector3 &norm);

        FrustumPlane(float a, float b, float c, float d);

        Vector3 normal;
        float distance;

        static auto intersection(const FrustumPlane &p1, const FrustumPlane &p2, const FrustumPlane &p3) -> Vector3;
        auto to_string() const -> std::string;
    };
}
