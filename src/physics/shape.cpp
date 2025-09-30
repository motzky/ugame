#include "physics/shape.h"

#include "physics/physics_sytem.h"
#include "utils/pass_key.h"

namespace game
{
    Shape::Shape(ShapeType type, PassKey<PhysicsSystem>)
        : _type(type)
    {
    }

    auto Shape::type() const -> ShapeType
    {
        return _type;
    }
}