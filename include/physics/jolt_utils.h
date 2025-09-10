#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Core/Color.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "ensure.h"
#include "log.h"
#include "math/vector3.h"
#include "primitives/color.h"
#include "primitives/line_data.h"

namespace game
{
    auto to_native(::JPH::RVec3Arg v) -> game::Vector3;
    auto to_native(::JPH::ColorArg c) -> game::Color;
    auto to_jolt(const Vector3 &v) -> ::JPH::RVec3;
    // auto to_jolt(const Color &c) -> ::JPH::Color;

}