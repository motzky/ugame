#pragma once

#include <format>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#include "pass_key.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"

namespace game
{
    class PhysicsSystem;

    class CylinderShape : public Shape
    {
    public:
        CylinderShape(float half_height, float radius, PassKey<PhysicsSystem>);

        auto half_height() const -> float;
        auto radius() const -> float;

        virtual auto native_handle() const -> const ::JPH::ShapeSettings * override;

    private:
        float _half_height;
        float _radius;
        ::JPH::CylinderShapeSettings _shape_settings;
    };
}

template <>
struct std::formatter<game::CylinderShape>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }
    auto format(const game::CylinderShape &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "CylinderShape: {} {}", obj.half_height(), obj.radius());
    }
};