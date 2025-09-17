#include "graphics/shape_wireframe_renderer.h"

#include <print>
#include <ranges>
#include <vector>

#include "camera.h"
#include "game/aabb.h"
#include "game/frustum_plane.h"
#include "graphics/color.h"
#include "graphics/line_data.h"
#include "math/matrix3.h"
#include "math/vector3.h"

namespace game
{
    auto ShapeWireframeRenderer::draw(const Vector3 &start, const Vector3 &end, const Color &color) -> void
    {
        _lines.push_back({start, color});
        _lines.push_back({end, color});
    }

    auto ShapeWireframeRenderer::draw(const Camera &camera) -> void
    {
        const auto corners = camera.frustum_corners();

        const auto near_colour = Color{1.0f, 0.0f, 1.0f};
        const auto far_colour = Color{1.0f, 0.0f, 0.0f};
        const auto connect_colour = Color{1.0f, 1.0f, 0.0f};

        draw(corners[0], corners[1], near_colour);
        draw(corners[1], corners[2], near_colour);
        draw(corners[2], corners[3], near_colour);
        draw(corners[3], corners[0], near_colour);

        draw(corners[4], corners[5], far_colour);
        draw(corners[5], corners[6], far_colour);
        draw(corners[6], corners[7], far_colour);
        draw(corners[7], corners[4], far_colour);

        draw(corners[0], corners[4], connect_colour);
        draw(corners[1], corners[5], connect_colour);
        draw(corners[2], corners[6], connect_colour);
        draw(corners[3], corners[7], connect_colour);
    }

    auto ShapeWireframeRenderer::draw(const AABB &aabb) -> void
    {
        _lines.push_back({aabb.max, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.min.x, aabb.max.y, aabb.max.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.min.x, aabb.max.y, aabb.max.z}, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.min.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.min.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.max.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.max.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({aabb.max, {0.f, 1.f, 0.f}});

        _lines.push_back({aabb.max, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.max.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.min.x, aabb.max.y, aabb.max.z}, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.min.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.max.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.max.x, aabb.min.y, aabb.min.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.min.x, aabb.max.y, aabb.min.z}, {0.f, 1.f, 0.f}});
        _lines.push_back({aabb.min, {0.f, 1.f, 0.f}});

        _lines.push_back({aabb.min, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.min.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.min.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.max.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.max.x, aabb.min.y, aabb.max.z}, {0.f, 1.f, 0.f}});
        _lines.push_back({{aabb.max.x, aabb.min.y, aabb.min.z}, {0.f, 1.f, 0.f}});

        _lines.push_back({{aabb.max.x, aabb.min.y, aabb.min.z}, {0.f, 1.f, 0.f}});
        _lines.push_back({aabb.min, {0.f, 1.f, 0.f}});
    }

    auto ShapeWireframeRenderer::yield() -> std::vector<LineData>
    {
        auto tmp = std::vector<LineData>{};
        std::ranges::swap(tmp, _lines);
        return tmp;
    }

}