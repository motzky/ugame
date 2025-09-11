#include "physics/sphere_shape.h"

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "ensure.h"
#include "math/vector3.h"
#include "physics/jolt_utils.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

namespace game
{
    SphereShape::SphereShape(float radius, PassKey<PhysicsSystem> pk)
        : Shape(pk),
          _radius(radius),
          _shape_settings(::JPH::SphereShapeSettings{radius})

    {
        _shape_settings.SetEmbedded();
    }

    auto SphereShape::radius() const -> float
    {
        return _radius;
    }

    auto SphereShape::native_handle() const -> const ::JPH::ShapeSettings *
    {
        return std::addressof(_shape_settings);
    }
}