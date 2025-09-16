#include "graphics/shape_wireframe_renderer.h"

#include <ranges>
#include <vector>

#include "camera.h"
#include "game/aabb.h"
#include "graphics/color.h"
#include "graphics/line_data.h"

namespace game
{
    auto ShapeWireframeRenderer::draw(const Camera & /*camera*/) -> void
    {
        // auto col = Color{1.f, 0.f, 1.f};
        // auto planes = camera.calculate_frustum_planes();

        // _lines.push_back({camera.position(), col});
        // _lines.push_back({planes[0].normal, col});

        // _lines.push_back({camera.position(), col});

        // _lines.push_back({camera.position(), col});

        // _lines.push_back({camera.position(), col});
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