#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Core/Color.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "ensure.h"
#include "graphics/color.h"
#include "graphics/line_data.h"
#include "log.h"
#include "math/quaternion.h"
#include "math/vector3.h"
#include "physics/physics_sytem.h"
#include "physics/rigid_body.h"

namespace game
{
    auto to_native(::JPH::RVec3Arg v) -> Vector3;
    auto to_native(::JPH::QuatArg &q) -> Quaternion;
    auto to_native(::JPH::ColorArg c) -> Color;

    auto to_jolt(const Vector3 &v) -> ::JPH::RVec3;
    // auto to_jolt(const Color &c) -> ::JPH::Color;
    auto to_jolt(const Quaternion &q) -> ::JPH::Quat;
    auto to_jolt(RigidBodyType type) -> ::JPH::EMotionType;
    auto to_jolt(PhysicsLayer) -> ::JPH::ObjectLayer;
    auto to_jolt_layer(RigidBodyType type) -> ::JPH::ObjectLayer;

    auto to_layer(RigidBodyType type) -> PhysicsLayer;
    auto to_activation(RigidBodyType type) -> ::JPH::EActivation;

}