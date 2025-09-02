#include "entity.h"

#include <span>

#include "material.h"
#include "matrix4.h"
#include "mesh.h"
#include "texture.h"
#include "texture_sampler.h"

namespace game
{
    Entity::Entity(const Mesh *mesh, const Material *material, const Vector3 &position, std::vector<const Texture *> textures, const TextureSampler *sampler)
        : _mesh(mesh), _material(material), _model(position), _textures(textures), _sampler(sampler)
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
    auto Entity::model() const -> std::span<const float>
    {
        return _model.data();
    }
    auto Entity::textures() const -> const std::vector<const Texture *> &
    {
        return _textures;
    }
    auto Entity::sampler() const -> const TextureSampler *
    {
        return _sampler;
    }
}