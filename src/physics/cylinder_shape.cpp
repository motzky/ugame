#include "physics/cylinder_shape.h"

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

namespace game
{
    CylinderShape::CylinderShape(float half_height, float radius, PassKey<PhysicsSystem> pk)
        : Shape(pk),
          _half_height(half_height),
          _radius(radius),
          _shape_settings{_half_height, _radius}

    {
        _shape_settings.SetEmbedded();
    }

    auto CylinderShape::half_height() const -> float
    {
        return _half_height;
    }

    auto CylinderShape::radius() const -> float
    {
        return _radius;
    }

    auto CylinderShape::native_handle() const -> const ::JPH::ShapeSettings *
    {
        return std::addressof(_shape_settings);
    }
}