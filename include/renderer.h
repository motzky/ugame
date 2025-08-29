#pragma once

#include "camera.h"
#include "material.h"
#include "mesh.h"

namespace game
{
    class Renderer
    {
    public:
        Renderer(Material material);
        auto render(const Camera &camera) const -> void;

    private:
        Material _material;
        Mesh _mesh;
    };
}