#pragma once

#include <vector>

#include "graphics/camera.h"
#include "graphics/color.h"
#include "graphics/line_data.h"

namespace game
{
    class ShapeWireframeRenderer
    {
    public:
        auto draw(const Vector3 &start, const Vector3 &end, const Color &color) -> void;
        auto draw(const Camera &camera) -> void;
        auto yield() -> std::vector<LineData>;

    private:
        std::vector<LineData> _lines;
    };
}
