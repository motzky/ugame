#pragma once

#include <span>

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
        Entity(const Mesh *mesh, const Material *material, const Vector3 &position, const Texture *texture, const TextureSampler *sampler);

        auto mesh() const -> const Mesh *;
        auto material() const -> const Material *;
        auto model() const -> std::span<const float>;
        auto texture() const -> const Texture *;
        auto sampler() const -> const TextureSampler *;

    private:
        const Mesh *_mesh;
        const Material *_material;
        Matrix4 _model;
        const Texture *_texture;
        const TextureSampler *_sampler;
    };

}
