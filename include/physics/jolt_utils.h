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
#include "math/vector3.h"
#include "physics/rigid_body.h"

namespace game
{
    auto to_native(::JPH::RVec3Arg v) -> game::Vector3;
    auto to_native(::JPH::ColorArg c) -> game::Color;

    auto to_jolt(const Vector3 &v) -> ::JPH::RVec3;
    // auto to_jolt(const Color &c) -> ::JPH::Color;
    auto to_jolt(RigidBodyType type) -> ::JPH::EMotionType;
    auto to_layer(game::RigidBodyType type) -> JPH::ObjectLayer;
    auto to_activation(game::RigidBodyType type) -> ::JPH::EActivation;

}