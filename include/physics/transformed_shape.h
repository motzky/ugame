#pragma once

#include "math/transform.h"
#include "physics/debug_renderer.h"
#include "physics/shape.h"

namespace game
{
    class TransformedShape
    {
    public:
        TransformedShape(const Shape *shape, const Transform &transform);
        auto intersects(const TransformedShape &shape) const -> bool;
        auto draw(DebugRenderer &debug_renderer) const -> void;

    private:
        const Shape *_shape;
        Transform _transform;
    };
}