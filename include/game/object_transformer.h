#pragma once

#include "math/vector3.h"

namespace game
{
    class ObjectTransformer
    {
    public:
        virtual ~ObjectTransformer() = default;

        virtual auto update() -> void = 0;
        virtual auto position() const -> Vector3 = 0;

    private:
    };
}