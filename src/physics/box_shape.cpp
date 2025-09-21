#include "physics/box_shape.h"

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include "math/vector3.h"
#include "physics/jolt_utils.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "utils/ensure.h"
#include "utils/pass_key.h"

namespace game
{
    BoxShape::BoxShape(const Vector3 &dimensions, PassKey<PhysicsSystem> pk)
        : Shape(pk),
          _dimensions(dimensions),
          _shape_settings(::JPH::BoxShapeSettings{to_jolt(_dimensions)})

    {
        _shape_settings.SetEmbedded();
    }

    auto BoxShape::dimensions() const -> Vector3
    {
        return _dimensions;
    }

    auto BoxShape::native_handle() const -> const ::JPH::ShapeSettings *
    {
        return std::addressof(_shape_settings);
    }
}