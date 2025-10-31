#include "physics/transformed_shape.h"

#include <optional>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/CollisionDispatch.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

#include "graphics/color.h"
#include "log.h"
#include "math/transform.h"
#include "physics/jolt_utils.h"
#include "physics/shape.h"
#include "utils/ensure.h"

namespace
{
    struct SimpleCollideShapeCollector : public ::JPH::CollideShapeCollector
    {
        SimpleCollideShapeCollector() : collision_result{std::nullopt} {}

        virtual auto AddHit(const ResultType &jolt_result) -> void override
        {
            collision_result = {
                .contact1 = game::to_native(jolt_result.mContactPointOn1),
                .contact2 = game::to_native(jolt_result.mContactPointOn2),
                .penetration_axis = game::to_native(jolt_result.mPenetrationAxis),
                .penetration_depth = jolt_result.mPenetrationDepth};
        }

        virtual auto OnBody(const ::JPH::Body &) -> void override
        {
            game::log::debug("collision OnBody");
        }
        std::optional<game::CollisionResult> collision_result;
    };

}

namespace game
{
    TransformedShape::TransformedShape(const Shape *shape, const Transform &transform)
        : _shape{shape},
          _transform{transform}
    {
    }

    auto TransformedShape::intersects(const TransformedShape &shape) const -> std::optional<CollisionResult>
    {
        auto collector = SimpleCollideShapeCollector{};
        const auto settings = ::JPH::CollideShapeSettings{};

        ::JPH::CollisionDispatch::sCollideShapeVsShape(
            _shape->native_handle(),
            shape._shape->native_handle(),
            to_jolt(_transform.scale),
            to_jolt(shape._transform.scale),
            to_jolt(_transform),
            to_jolt(shape._transform),
            ::JPH::SubShapeIDCreator{},
            ::JPH::SubShapeIDCreator{},
            settings,
            collector);

        return collector.collision_result;
    }

    auto TransformedShape::draw(DebugRenderer &debug_renderer, const Color &color) const -> void
    {
        if (!_shape)
        {
            return;
        }
        _shape->native_handle()->Draw(
            std::addressof(debug_renderer),
            to_jolt(_transform),
            ::JPH::RVec3::sOne(),
            to_jolt(color),
            false, true);
    }

    auto TransformedShape::set_position(const Vector3 &position) -> void
    {
        _transform.position = position;
    }

    auto TransformedShape::translate(const Vector3 &translation) -> void
    {
        _transform.position += translation;
    }

    auto TransformedShape::shape() const -> const Shape *
    {
        return _shape;
    }

    auto TransformedShape::transform() const -> const Transform &
    {
        return _transform;
    }
}