#pragma once

#include <vector>
#include <tuple>

#include "material.h"
#include "matrix4.h"
#include "mesh.h"
#include "texture.h"
#include "texture_sampler.h"

namespace game
{
    class Entity
    {
    public:
        Entity(const Mesh *mesh, const Material *material, const Vector3 &position, const Vector3 &scale, std::vector<std::tuple<const Texture *, const TextureSampler *>> &textures);

        auto mesh() const -> const Mesh *;
        auto material() const -> const Material *;
        auto model() const -> const Matrix4 &;
        auto textures() const -> std::span<const std::tuple<const Texture *, const TextureSampler *>>;

    private:
        const Mesh *_mesh;
        const Material *_material;
        Matrix4 _model;
        std::vector<std::tuple<const Texture *, const TextureSampler *>> _textures;
    };

}
