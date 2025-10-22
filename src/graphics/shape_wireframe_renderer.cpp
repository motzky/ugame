#include "graphics/shape_wireframe_renderer.h"

#include <vector>

#include "graphics/camera.h"
#include "graphics/color.h"
#include "graphics/line_data.h"
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

    auto ShapeWireframeRenderer::yield() -> std::vector<LineData>
    {
        auto tmp = std::vector<LineData>{};
        std::ranges::swap(tmp, _lines);
        return tmp;
    }

}