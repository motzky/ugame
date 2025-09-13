#pragma once

#include "game/object_transformer.h"
#include "math/vector3.h"

namespace game
{
    class StaticObjectTransformer : public ObjectTransformer
    {
    public:
        StaticObjectTransformer(const Vector3 &position);
        ~StaticObjectTransformer() override = default;

        virtual auto update() -> void override;
        virtual auto position() const -> Vector3 override;

    private:
        Vector3 _position;
    };
}