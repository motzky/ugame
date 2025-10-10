#pragma once

#include <optional>
#include <vector>

#include "graphics/color.h"
#include "graphics/cube_map.h"
#include "graphics/line_data.h"
#include "graphics/texture_sampler.h"
#include "graphics/ui/label.h"
#include "math/vector3.h"
#include "primitives/debug_lines.h"
#include "primitives/entity.h"

namespace game
{
    struct DirectionalLight
    {
        Vector3 direction;
        Color color;
    };

    struct PointLight
    {
        Vector3 position;
        Color color;
        float const_attenuation;
        float linear_attenuation;
        float quad_attenuation;
    };

    struct Scene
    {
        std::vector<Entity *> entities;
        Color ambient;
        DirectionalLight directional;
        std::vector<PointLight> points;
        std::optional<DebugLines> debug_lines;
        const CubeMap *skybox;
        const TextureSampler *skybox_sampler;
        std::vector<graphics::ui::Label> labels;
    };
}