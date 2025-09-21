#include "math/vector3.h"

namespace game
{
    auto Vector3::distance(const Vector3 &v1, const Vector3 &v2) -> float
    {
        return (v2 - v1).length();
    }

}