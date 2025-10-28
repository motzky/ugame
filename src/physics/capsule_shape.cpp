#include "physics/capsule_shape.h"

#include <format>
#include <string>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

namespace game
{
    CapsuleShape::CapsuleShape(float half_height, float radius, PassKey<PhysicsSystem> pk)
        : Shape(ShapeType::CAPSULE, pk),
          _half_height(half_height),
          _radius(radius),
          _shape_settings{_half_height, _radius},
          _shape(half_height, radius)

    {
        _shape_settings.SetEmbedded();
    }

    auto CapsuleShape::half_height() const -> float
    {
        return _half_height;
    }

    auto CapsuleShape::radius() const -> float
    {
        return _radius;
    }

    auto CapsuleShape::native_handle() const -> const ::JPH::Shape *
    {
        return std::addressof(_shape);
    }

    auto CapsuleShape::to_string() const -> std::string
    {
        return std::format("CapsuleShape: {} {}", _half_height, _radius);
    }

}