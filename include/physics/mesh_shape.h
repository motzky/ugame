#pragma once

#include <string>
#include <vector>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include "graphics/mesh_data.h"
#include "math/vector3.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;

    class MeshShape : public Shape
    {
    public:
        MeshShape(MeshData mesh_data, PassKey<PhysicsSystem>);

        virtual auto native_handle() const -> const ::JPH::Shape * override;

        auto to_string() const -> std::string;

    private:
        ::JPH::Ref<::JPH::Shape> _shape;
    };
}
