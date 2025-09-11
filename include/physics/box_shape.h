#pragma once

#include <format>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include "math/vector3.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;

    class BoxShape : public Shape
    {
    public:
        BoxShape(const Vector3 &dimensions, PassKey<PhysicsSystem>);

        auto dimensions() const -> Vector3;

        virtual auto native_handle() const -> const ::JPH::ShapeSettings * override;

    private:
        Vector3 _dimensions;
        ::JPH::BoxShapeSettings _shape_settings;
    };
}

template <>
struct std::formatter<game::BoxShape>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }
    auto format(const game::BoxShape &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "BoxShape: {}", obj.dimensions());
    }
};