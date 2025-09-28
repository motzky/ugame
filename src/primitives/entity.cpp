#include "primitives/entity.h"

#include <ranges>
#include <span>
#include <vector>

#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "math/aabb.h"
#include "math/matrix4.h"
#include "math/quaternion.h"
#include "math/vector3.h"

namespace
{
    auto calculate_bounding_box(const game::Mesh *mesh, const game::Transform &transform) -> game::AABB
    {
        const auto minmax_x = std::ranges::minmax(mesh->mesh_data().vertices, std::less<>{}, [](const auto &v)
                                                  { return v.position.x; });
        const auto minmax_y = std::ranges::minmax(mesh->mesh_data().vertices, std::less<>{}, [](const auto &v)
                                                  { return v.position.y; });
        const auto minmax_z = std::ranges::minmax(mesh->mesh_data().vertices, std::less<>{}, [](const auto &v)
                                                  { return v.position.z; });

        const auto transformed_min = game::Matrix4{transform} * game::Vector4{minmax_x.min.position.x, minmax_y.min.position.y, minmax_z.min.position.z, 1.0};
        const auto transformed_max = game::Matrix4{transform} * game::Vector4{minmax_x.max.position.x, minmax_y.max.position.y, minmax_z.max.position.z, 1.0};
        auto aabb = game::AABB{static_cast<game::Vector3>(transformed_min), static_cast<game::Vector3>(transformed_max)};

        return aabb;
    }
}

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
          _textures(std::ranges::cbegin(textures), std::ranges::cend(textures)),
          _visible(true),
          _aabb(calculate_bounding_box(mesh, _transform))
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
          _textures(std::ranges::cbegin(textures), std::ranges::cend(textures)),
          _visible(true),
          _aabb{}
    {
        _transform.rotation = _local_transform.rotation;
        _aabb = calculate_bounding_box(mesh, _transform);
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
        const auto delta = position - _transform.position;
        translate(delta);
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
    auto Entity::is_visible() const -> bool
    {
        return _visible;
    }
    auto Entity::set_visibility(bool visible) -> void
    {
        _visible = visible;
    }

    auto Entity::bounding_box() const -> AABB
    {
        return _aabb;
    }

    auto Entity::translate(const Vector3 &translation) -> void
    {
        _transform.position += _local_transform.position + translation;
        _aabb.min += translation;
        _aabb.max += translation;
    }
}