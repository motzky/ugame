#include "physics/jolt_utils.h"

#include <Jolt/Jolt.h>

#include <Jolt/Core/Color.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "ensure.h"
#include "log.h"
#include "math/vector3.h"
#include "physics/rigid_body.h"
#include "primitives/color.h"
#include "primitives/line_data.h"

namespace game
{
    auto to_native(::JPH::RVec3Arg v) -> game::Vector3
    {
        return {v.GetX(), v.GetY(), v.GetZ()};
    }

    auto to_native(::JPH::ColorArg c) -> game::Color
    {
        const auto v = c.ToVec4();
        return {v.GetX(), v.GetY(), v.GetZ()};
    }

    auto to_jolt(const Vector3 &v) -> ::JPH::RVec3
    {
        return {v.x, v.y, v.z};
    }

    // auto to_jolt(const Color &c) -> ::JPH::Color
    // {
    //     return ::JPH::Color();
    // }

    auto to_jolt(RigidBodyType type) -> ::JPH::EMotionType
    {
        switch (type)
        {
            using enum game::RigidBodyType;
        case STATIC:
            return ::JPH::EMotionType::Static;
        case DYNAMIC:
            return ::JPH::EMotionType::Dynamic;

        default:
            throw game::Exception("unknown RigidBodyType");
        }
    }

    auto to_layer(game::RigidBodyType type) -> JPH::ObjectLayer
    {
        switch (type)
        {
            using enum game::RigidBodyType;
        case STATIC:
            return 1u;
        case DYNAMIC:
            return 0u;

        default:
            throw game::Exception("unknown RigidBodyType");
        }
    }

    auto to_activation(game::RigidBodyType type) -> ::JPH::EActivation
    {
        switch (type)
        {
            using enum game::RigidBodyType;
        case STATIC:
            return ::JPH::EActivation::DontActivate;
        case DYNAMIC:
            return ::JPH::EActivation::Activate;

        default:
            throw game::Exception("unknown RigidBodyType");
        }
    }
}