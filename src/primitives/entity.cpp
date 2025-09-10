#include "primitives/entity.h"

#include <span>
#include <tuple>
#include <vector>

#include "material.h"
#include "math/vector3.h"
#include "mesh.h"
#include "texture.h"
#include "texture_sampler.h"

namespace game
{
    Entity::Entity(const Mesh *mesh, const Material *material, const Vector3 &position, const Vector3 &scale, std::vector<std::tuple<const Texture *, const TextureSampler *>> &textures)
        : _mesh(mesh), _material(material), _model(position, scale), _textures(textures)
    {
    }

    auto Entity::mesh() const -> const Mesh *
    {
        return _mesh;
    }
    auto Entity::material() const -> const Material *
    {
        return _material;
    }
    auto Entity::model() const -> const Matrix4 &
    {
        return _model;
    }
    auto Entity::textures() const -> std::span<const std::tuple<const Texture *, const TextureSampler *>>
    {
        return _textures;
    }
}