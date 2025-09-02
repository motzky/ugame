#pragma once

#include <span>
#include <vector>

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
        Entity(const Mesh *mesh, const Material *material, const Vector3 &position, std::vector<const Texture *> textures, const TextureSampler *sampler);

        auto mesh() const -> const Mesh *;
        auto material() const -> const Material *;
        auto model() const -> std::span<const float>;
        auto textures() const -> const std::vector<const Texture *> &;
        auto sampler() const -> const TextureSampler *;

    private:
        const Mesh *_mesh;
        const Material *_material;
        Matrix4 _model;
        std::vector<const Texture *> _textures;
        const TextureSampler *_sampler;
    };

}
