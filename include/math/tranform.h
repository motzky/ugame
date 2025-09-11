#pragma once

#include "math/matrix4.h"
#include "math/quaternion.h"
#include "math/vector3.h"

namespace game
{
    class Transform
    {
    public:
        Transform()
            : Transform({0.f}, {1.f})
        {
        }
        Transform(const Vector3 &position, const Vector3 &scale)
            : Transform(position, scale, {0.f, 0.f, 0.f, 1.f})
        {
        }
        Transform(const Vector3 &position, const Vector3 &scale, const Quaternion &rotation)
            : position(position),
              scale(scale),
              rotation(rotation)
        {
        }

        constexpr operator Matrix4() const
        {
            return Matrix4{position} * Matrix4{rotation} * Matrix4{scale, Matrix4::Scale{}};
        }

        Vector3 position;
        Vector3 scale;
        Quaternion rotation;
    };
}