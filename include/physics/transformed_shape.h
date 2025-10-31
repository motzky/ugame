#pragma once

#include <optional>

#include "math/transform.h"
#include "math/vector3.h"
#include "physics/debug_renderer.h"
#include "physics/shape.h"

namespace game
{
    struct CollisionResult
    {
        Vector3 contact1;
        Vector3 contact2;
        Vector3 penetration_axis;
        float penetration_depth;
    };

    class TransformedShape
    {
    public:
        TransformedShape(const Shape *shape, const Transform &transform);
        auto intersects(const TransformedShape &shape) const -> std::optional<CollisionResult>;

        auto draw(DebugRenderer &debug_renderer, const Color &color) const -> void;

        auto set_position(const Vector3 &position) -> void;
        auto translate(const Vector3 &translation) -> void;

        auto shape() const -> const Shape *;
        auto transform() const -> const Transform &;

    private:
        const Shape *_shape;
        Transform _transform;
    };
}