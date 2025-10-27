#pragma once

#include <cstdint>
#include <optional>
#include <tuple>
#include <vector>

#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "math/quaternion.h"
#include "math/transform.h"
#include "math/vector3.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "physics/transformed_shape.h"

namespace game
{
    class Entity
    {
    public:
        Entity(const Mesh *mesh,
               const Material *material,
               const Vector3 &position,
               const Vector3 &scale,
               std::span<const Texture *const> textures,
               TransformedShape bounding_box,
               std::uint32_t collision_layer,
               std::uint32_t collision_mask,
               std::optional<TransformedShape> static_collider = std::nullopt);
        Entity(const Mesh *mesh,
               const Material *material,
               const Vector3 &position,
               const Vector3 &scale,
               const Transform &local_transform,
               std::span<const Texture *const> textures,
               TransformedShape bounding_box,
               std::uint32_t collision_layer,
               std::uint32_t collision_mask,
               std::optional<TransformedShape> static_collider = std::nullopt);

        auto mesh() const -> const Mesh *;
        auto material() const -> const Material *;
        auto textures() const -> std::span<const Texture *const>;
        auto transform() const -> const Transform &;
        auto local_transform() const -> const Transform &;
        auto position() const -> Vector3;
        auto is_visible() const -> bool;
        auto set_visibility(bool visible) -> void;
        auto bounding_box() const -> const TransformedShape &;
        auto static_collider() const -> std::optional<TransformedShape>;

        auto set_position(const Vector3 &position) -> void;
        auto set_rotation(const Quaternion &rotation) -> void;
        auto translate(const Vector3 &translation) -> void;

        auto collision_layer() const -> std::uint32_t;
        auto collision_mask() const -> std::uint32_t;

    private:
        const Mesh *_mesh;
        const Material *_material;
        Transform _transform;
        Transform _local_transform;
        std::vector<const Texture *> _textures;
        bool _visible;
        TransformedShape _bounding_box;
        std::uint32_t _collision_layer;
        std::uint32_t _collision_mask;
        std::optional<TransformedShape> _static_collider;
    };

}
