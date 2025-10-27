#include "physics/mesh_shape.h"

#include <format>
#include <ranges>
#include <string>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Array.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include "graphics/mesh_data.h"
#include "physics/jolt_utils.h"
#include "physics/shape.h"
#include "utils/ensure.h"
#include "utils/pass_key.h"

namespace
{
    auto create_mesh_shape(game::MeshData mesh_data) -> ::JPH::Ref<::JPH::Shape>
    {
        auto jolt_vertex_list =
            mesh_data.vertices |
            std::views::transform([](const auto &e)
                                  { return ::JPH::Float3{e.position.x, e.position.y, e.position.z}; }) |
            std::ranges::to<::JPH::VertexList>();

        auto jolt_index_list =
            mesh_data.indices |
            std::views::chunk(3u) |
            std::views::transform([](const auto &e)
                                  { return ::JPH::IndexedTriangle{e[0], e[1], e[2]}; }) |
            std::ranges::to<::JPH::IndexedTriangleList>();

        auto settings = ::JPH::MeshShapeSettings{jolt_vertex_list, jolt_index_list};

        auto result = settings.Create();
        game::ensure(result.IsValid(), "failed to create mesh shape: {}", result.GetError());

        return result.Get();
    }
}

namespace game
{
    MeshShape::MeshShape(MeshData mesh_data, PassKey<PhysicsSystem> pk)
        : Shape(ShapeType::MESH, pk),
          _shape{create_mesh_shape(mesh_data)}

    {
    }

    auto MeshShape::native_handle() const -> const ::JPH::Shape *
    {
        return _shape.GetPtr();
    }

    auto MeshShape::to_string() const -> std::string
    {
        // return std::format("MeshShape: {}", _dimensions);
        return "MeshShape";
    }

}