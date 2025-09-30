#include "physics/transformed_shape.h"

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/CollisionDispatch.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

#include "log.h"
#include "math/transform.h"
#include "physics/jolt_utils.h"
#include "physics/shape.h"
#include "utils/ensure.h"

namespace
{
    class SimpleCollideShapeCollector : public ::JPH::CollideShapeCollector
    {
        virtual auto AddHit([[maybe_unused]] const ::JPH::CollideShapeCollector::ResultType &inResult) -> void override
        {
            game::log::debug("collision AddHit");
        }

        virtual auto OnBody(const ::JPH::Body &) -> void override
        {
            game::log::debug("collision OnBody");
        }
    };

}

namespace game
{
    TransformedShape::TransformedShape(const Shape *shape, const Transform &transform)
        : _shape{shape},
          _transform{transform}
    {
    }

    auto TransformedShape::intersects(const TransformedShape &shape) const -> bool
    {
        auto collector = SimpleCollideShapeCollector{};
        const auto settings = ::JPH::CollideShapeSettings{};

        ::JPH::CollisionDispatch::sCollideShapeVsShape(
            _shape->native_handle(),
            shape._shape->native_handle(),
            ::JPH::RVec3::sOne(),
            ::JPH::RVec3::sOne(),
            to_jolt(_transform),
            to_jolt(shape._transform),
            ::JPH::SubShapeIDCreator{},
            ::JPH::SubShapeIDCreator{},
            settings,
            collector);

        return false;
    }

    auto TransformedShape::draw(DebugRenderer &debug_renderer) const -> void
    {
        _shape->native_handle()->Draw(
            std::addressof(debug_renderer),
            to_jolt(_transform),
            ::JPH::RVec3{2.f, 2.f, 2.f},

            ::JPH::Color{255, 0, 255},
            false, true);
    }
}