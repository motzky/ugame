#pragma once

#include <tuple>
#include <vector>

#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "math/quaternion.h"
#include "math/tranform.h"
#include "math/vector3.h"

namespace game
{
    class Entity
    {
    public:
        Entity(const Mesh *mesh,
               const Material *material,
               const Vector3 &position,
               const Vector3 &scale,
               std::vector<std::tuple<const Texture *, const TextureSampler *>> &textures);

        auto mesh() const -> const Mesh *;
        auto material() const -> const Material *;
        auto textures() const -> std::span<const std::tuple<const Texture *, const TextureSampler *>>;
        auto transform() const -> const Transform &;

        auto set_position(const Vector3 &position) -> void;
        auto set_rotation(const Quaternion &rotation) -> void;

    private:
        const Mesh *_mesh;
        const Material *_material;
        Transform _transform;
        std::vector<std::tuple<const Texture *, const TextureSampler *>> _textures;
    };

}
