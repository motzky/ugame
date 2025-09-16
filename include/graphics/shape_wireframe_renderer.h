#pragma once

#include <vector>

#include "camera.h"
#include "game/aabb.h"
#include "graphics/line_data.h"

namespace game
{
    class ShapeWireframeRenderer
    {
    public:
        auto draw(const Camera &camera) -> void;
        auto draw(const AABB &aabb) -> void;
        auto yield() -> std::vector<LineData>;

    private:
        std::vector<LineData> _lines;
    };
}