#pragma once

#ifndef JPH_DEBUG_RENDERER
#error This file should only be included when JPH_DEBUG_RENDERER is defined
#endif // !JPH_DEBUG_RENDERER

#include <span>
#include <string_view>
#include <vector>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Color.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "ensure.h"
#include "graphics/line_data.h"
#include "log.h"
#include "pass_key.h"

namespace game
{
    class PhysicsSystem;

    class DebugRenderer : public ::JPH::DebugRendererSimple
    {
    public:
        DebugRenderer(PassKey<PhysicsSystem>);

        virtual auto DrawLine(::JPH::RVec3Arg from, ::JPH::RVec3Arg to, ::JPH::ColorArg color) -> void override;
        virtual auto DrawTriangle(::JPH::RVec3Arg v1, ::JPH::RVec3Arg v2, ::JPH::RVec3Arg v3, ::JPH::ColorArg color, ::JPH::DebugRenderer::ECastShadow castShadow) -> void override;
        virtual auto DrawText3D(::JPH::RVec3Arg position, const std::string_view &string, ::JPH::ColorArg color, float height) -> void override;

        auto clear() -> void;

        auto lines() const -> std::span<const LineData>;

    private:
        std::vector<LineData> _lines;
    };
}