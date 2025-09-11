#pragma once

#include <tuple>
#include <vector>

#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "math/vector3.h"

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

        auto set_position(const Vector3 &position) -> void;

    private:
        const Mesh *_mesh;
        const Material *_material;
        Matrix4 _model;
        std::vector<std::tuple<const Texture *, const TextureSampler *>> _textures;
        // const Vector3 &_scale;
    };

}
