#include "primitives/entity.h"

#include <ranges>
#include <span>
#include <tuple>
#include <vector>

#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "math/quaternion.h"
#include "math/vector3.h"

namespace game
{
    Entity::Entity(const Mesh *mesh,
                   const Material *material,
                   const Vector3 &position,
                   const Vector3 &scale,
                   std::span<const Texture *const> textures)
        : _mesh(mesh),
          _material(material),
          _transform(position, scale),
          _local_transform(),
          _textures(std::ranges::cbegin(textures), std::ranges::cend(textures))
    {
    }

    Entity::Entity(const Mesh *mesh,
                   const Material *material,
                   const Vector3 &position,
                   const Vector3 &scale,
                   const Transform &local_transform,
                   std::span<const Texture *const> textures)
        : _mesh(mesh),
          _material(material),
          _transform(position, scale),
          _local_transform(local_transform),
          _textures(std::ranges::cbegin(textures), std::ranges::cend(textures))
    {
        _transform.rotation = _local_transform.rotation;
    }

    auto Entity::mesh() const -> const Mesh *
    {
        return _mesh;
    }
    auto Entity::material() const -> const Material *
    {
        return _material;
    }
    auto Entity::set_position(const Vector3 &position) -> void
    {
        _transform.position = position;
    }
    auto Entity::set_rotation(const Quaternion &rotation) -> void
    {
        _transform.rotation = rotation * _local_transform.rotation;
    }
    auto Entity::textures() const -> std::span<const Texture *const>
    {
        return _textures;
    }
    auto Entity::transform() const -> const Transform &
    {
        return _transform;
    }
    auto Entity::local_transform() const -> const Transform &
    {
        return _local_transform;
    }
    auto Entity::position() const -> Vector3
    {
        return _transform.position;
    }
    auto Entity::translate(const Vector3 &translation) -> void
    {
        _transform.position += _local_transform.position + translation;
    }
}