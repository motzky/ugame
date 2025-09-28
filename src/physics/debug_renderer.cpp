#include "physics/debug_renderer.h"

#include <string_view>
#include <vector>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Color.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "graphics/line_data.h"
#include "log.h"
#include "physics/jolt_utils.h"
#include "utils/ensure.h"

namespace game
{
    DebugRenderer::DebugRenderer(PassKey<PhysicsSystem>)
    {
    }

    auto DebugRenderer::DrawLine(::JPH::RVec3Arg from, ::JPH::RVec3Arg to, ::JPH::ColorArg color) -> void
    {
        _lines.push_back({to_native(from), to_native(color)});
        _lines.push_back({to_native(to), to_native(color)});
    }

    auto DebugRenderer::DrawTriangle(::JPH::RVec3Arg v1,
                                     ::JPH::RVec3Arg v2,
                                     ::JPH::RVec3Arg v3,
                                     ::JPH::ColorArg color,
                                     [[maybe_unused]] ::JPH::DebugRenderer::ECastShadow castShadow) -> void
    {
        DrawLine(v1, v2, color);
        DrawLine(v2, v3, color);
        DrawLine(v3, v1, color);
    }

    auto DebugRenderer::DrawText3D([[maybe_unused]] ::JPH::RVec3Arg position,
                                   [[maybe_unused]] const std::string_view &text,
                                   [[maybe_unused]] ::JPH::ColorArg color,
                                   [[maybe_unused]] float height) -> void
    {
        log::warn("debug text {}", text);
    }

    auto DebugRenderer::clear() -> void
    {
        _lines.clear();
    }

    auto DebugRenderer::lines() const -> std::span<const LineData>
    {
        return _lines;
    }
}