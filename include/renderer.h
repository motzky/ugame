#pragma once

#include "material.h"
#include "mesh.h"

namespace game
{
    class Renderer
    {
    public:
        Renderer(Material material);
        auto render() const -> void;

    private:
        Material _material;
        Mesh _mesh;
    };
}