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
               std::span<const Texture *> textures);
        Entity(const Mesh *mesh,
               const Material *material,
               const Vector3 &position,
               const Vector3 &scale,
               const Transform &local_transform,
               std::span<const Texture *> textures);

        auto mesh() const -> const Mesh *;
        auto material() const -> const Material *;
        auto textures() const -> std::span<const Texture *const>;
        auto transform() const -> const Transform &;
        auto local_transform() const -> const Transform &;
        auto position() const -> Vector3;

        auto set_position(const Vector3 &position) -> void;
        auto set_rotation(const Quaternion &rotation) -> void;
        auto translate(const Vector3 &translation) -> void;

    private:
        const Mesh *_mesh;
        const Material *_material;
        Transform _transform;
        Transform _local_transform;
        std::vector<const Texture *> _textures;
    };

}
