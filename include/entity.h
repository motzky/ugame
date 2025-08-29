#pragma once

#include <span>

#include "material.h"
#include "matrix4.h"
#include "mesh.h"

namespace game
{
    class Entity
    {
    public:
        Entity(const Mesh *mesh, const Material *material);

        auto model() const -> std::span<const float>;

    private:
        const Mesh *_mesh;
        const Material *_material;
        Matrix4 _model;
    };

}
