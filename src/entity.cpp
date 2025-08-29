#include "entity.h"

#include <span>

#include "material.h"
#include "matrix4.h"
#include "mesh.h"

namespace game
{
    Entity::Entity(const Mesh *mesh, const Material *material) : _mesh(mesh), _material(material), _model()
    {
    }

    auto Entity::model() const -> std::span<const float>
    {
        return _model.data();
    }
}