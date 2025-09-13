#include "game/static_object_transformer.h"

#include "game/object_transformer.h"
#include "math/vector3.h"

namespace game
{
    StaticObjectTransformer::StaticObjectTransformer(const Vector3 &position)
        : _position{position}
    {
    }

    auto StaticObjectTransformer::update() -> void
    {
    }

    auto StaticObjectTransformer::position() const -> Vector3
    {
        return _position;
    }
}