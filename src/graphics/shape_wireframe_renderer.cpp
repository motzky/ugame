#include "graphics/shape_wireframe_renderer.h"

#include <vector>

#include "camera.h"
#include "graphics/color.h"
#include "graphics/line_data.h"
#include "math/aabb.h"
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
        auto color_aabb = Color{0.f, 1.f, 0.f};

        const auto aabb_min = aabb.get_min();
        const auto aabb_max = aabb.get_max();

        draw(aabb_max, {aabb_min.x, aabb_max.y, aabb_max.z}, color_aabb);
        draw({aabb_min.x, aabb_max.y, aabb_max.z}, {aabb_min.x, aabb_max.y, aabb_min.z}, color_aabb);
        draw({aabb_min.x, aabb_max.y, aabb_min.z}, {aabb_max.x, aabb_max.y, aabb_min.z}, color_aabb);
        draw({aabb_max.x, aabb_max.y, aabb_min.z}, aabb_max, color_aabb);
        draw(aabb_max, {aabb_max.x, aabb_min.y, aabb_max.z}, color_aabb);
        draw({aabb_min.x, aabb_max.y, aabb_max.z}, {aabb_min.x, aabb_min.y, aabb_max.z}, color_aabb);
        draw({aabb_max.x, aabb_max.y, aabb_min.z}, {aabb_max.x, aabb_min.y, aabb_min.z}, color_aabb);
        draw({aabb_min.x, aabb_max.y, aabb_min.z}, aabb_min, color_aabb);
        draw(aabb_min, {aabb_min.x, aabb_min.y, aabb_max.z}, color_aabb);
        draw({aabb_min.x, aabb_min.y, aabb_max.z}, {aabb_max.x, aabb_min.y, aabb_max.z}, color_aabb);
        draw({aabb_max.x, aabb_min.y, aabb_max.z}, {aabb_max.x, aabb_min.y, aabb_min.z}, color_aabb);
        draw({aabb_max.x, aabb_min.y, aabb_min.z}, aabb_min, color_aabb);
    }

    auto ShapeWireframeRenderer::yield() -> std::vector<LineData>
    {
        auto tmp = std::vector<LineData>{};
        std::ranges::swap(tmp, _lines);
        return tmp;
    }

}