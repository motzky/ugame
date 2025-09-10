#pragma once

#include <format>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "math/vector3.h"
#include "pass_key.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"

namespace game
{
    class PhysicsSystem;

    class SphereShape : public Shape
    {
    public:
        SphereShape(float radius, PassKey<PhysicsSystem>);

        auto radius() const -> float;

        virtual auto native_handle() const -> const ::JPH::ShapeSettings * override;

    private:
        float _radius;
        ::JPH::SphereShapeSettings _shape_settings;
    };
}

template <>
struct std::formatter<game::SphereShape>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }
    auto format(const game::SphereShape &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "SphereShape: {}", obj.radius());
    }
};