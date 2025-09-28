#include "physics/jolt_utils.h"

#include <ranges>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Color.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "graphics/color.h"
#include "graphics/line_data.h"
#include "log.h"
#include "math/matrix4.h"
#include "math/transform.h"
#include "math/vector3.h"
#include "physics/physics_sytem.h"
#include "physics/rigid_body.h"
#include "utils/ensure.h"

namespace game
{
    auto to_native(::JPH::RVec3Arg v) -> Vector3
    {
        return {v.GetX(), v.GetY(), v.GetZ()};
    }

    auto to_native(::JPH::QuatArg &q) -> Quaternion
    {
        return {q.GetX(), q.GetY(), q.GetZ(), q.GetW()};
    }

    auto to_native(::JPH::ColorArg c) -> Color
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

    auto to_jolt(const Quaternion &q) -> ::JPH::Quat
    {
        return ::JPH::Quat{q.x, q.y, q.z, q.w};
    }

    auto to_jolt(RigidBodyType type) -> ::JPH::EMotionType
    {
        switch (type)
        {
            using enum RigidBodyType;
        case STATIC:
            return ::JPH::EMotionType::Static;
        case DYNAMIC:
            return ::JPH::EMotionType::Dynamic;

        default:
            throw Exception("unknown RigidBodyType");
        }
    }

    auto to_layer(RigidBodyType type) -> PhysicsLayer
    {
        switch (type)
        {
            using enum RigidBodyType;
            using enum PhysicsLayer;
        case STATIC:
            return NON_MOVING;
        case DYNAMIC:
            return MOVING;

        default:
            throw Exception("unknown RigidBodyType");
        }
    }
    auto to_jolt(PhysicsLayer layer) -> ::JPH::ObjectLayer
    {
        return static_cast<::JPH::ObjectLayer>(layer);
    }

    auto to_jolt([[maybe_unused]] const Transform &transform) -> ::JPH::RMat44
    {
        const auto mat = static_cast<Matrix4>(transform);
        const auto chunks = mat.data() | std::views::chunk(4u);
        const auto cols = chunks | std::views::transform([](auto c)
                                                         { return ::JPH::Vec4{c[0], c[1], c[2], c[3]}; });

        return ::JPH::Mat44{cols[0], cols[1], cols[2], cols[3]};
    }

    auto to_jolt_layer(RigidBodyType type) -> ::JPH::ObjectLayer
    {
        return to_jolt(to_layer(type));
    }

    auto to_activation(RigidBodyType type) -> ::JPH::EActivation
    {
        switch (type)
        {
            using enum RigidBodyType;
        case STATIC:
            return ::JPH::EActivation::DontActivate;
        case DYNAMIC:
            return ::JPH::EActivation::Activate;

        default:
            throw Exception("unknown RigidBodyType");
        }
    }
}