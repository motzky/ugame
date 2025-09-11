#include "mesh_loader.h"

#include <cstdint>
#include <ranges>
#include <span>
#include <vector>

#include "ensure.h"
#include "graphics/vertex_data.h"
#include "log.h"
#include "math/vector3.h"
#include "resource_loader.h"
#include "string_unordered_map.h"

namespace
{
    template <class... Args>
    std::vector<game::VertexData> vertices(Args &&...args)
    {
        return std::views::zip_transform(
                   []<class... A>(A &&...a)
                   { return game::VertexData{std::forward<A>(a)...}; },
                   std::forward<Args>(args)...) |
               std::ranges::to<std::vector>();
    }
}

namespace game
{
    MeshLoader::MeshLoader(ResourceLoader &resource_loader)
        : _resource_loader(resource_loader)
    {
    }

    auto MeshLoader::sprite() -> MeshData
    {
        const auto loaded = _loaded_meshes.find("sprite");

        if (loaded != std::ranges::cend(_loaded_meshes))
        {
            return {.vertices = loaded->second.vertices, .indices = loaded->second.indices};
        }

        const Vector3 positions[] = {
            {-1.f, 1.f, 0.f},
            {-1.f, -1.f, 0.f},
            {1.f, -1.f, 0.f},
            {1.f, 1.f, 0.f}};

        const UV texture_coords[] = {
            {0.f, 1.f},
            {0.f, 0.f},
            {1.f, 0.f},
            {1.f, 1.f}};

        auto indices = std::vector<std::uint32_t>{0, 1, 2, 0, 2, 3};

        const auto new_item = _loaded_meshes.emplace("sprite", LoadedMeshData{vertices(positions, positions, positions, texture_coords), std::move(indices)});

        return {.vertices = new_item.first->second.vertices, .indices = new_item.first->second.indices};
    }

    auto MeshLoader::cube() -> MeshData
    {
        const auto loaded = _loaded_meshes.find("cube");

        if (loaded != std::ranges::cend(_loaded_meshes))
        {
            return {.vertices = loaded->second.vertices, .indices = loaded->second.indices};
        }

        const Vector3 positions[] = {
            {-1.f, -1.f, 1.f},
            {1.f, -1.f, 1.f},
            {1.f, 1.f, 1.f},
            {-1.f, 1.f, 1.f},
            {1.f, -1.f, 1.f},
            {1.f, -1.f, -1.f},
            {1.f, 1.f, -1.f},
            {1.f, 1.f, 1.f},
            {1.f, -1.f, -1.f},
            {-1.f, -1.f, -1.f},
            {-1.f, 1.f, -1.f},
            {1.f, 1.f, -1.f},
            {-1.f, -1.f, -1.f},
            {-1.f, -1.f, 1.f},
            {-1.f, 1.f, 1.f},
            {-1.f, 1.f, -1.f},
            {-1.f, 1.f, 1.f},
            {1.f, 1.f, 1.f},
            {1.f, 1.f, -1.f},
            {-1.f, 1.f, -1.f},
            {-1.f, -1.f, -1.f},
            {1.f, -1.f, -1.f},
            {1.f, -1.f, 1.f},
            {-1.f, -1.f, 1.f}};

        const Vector3 normals[] = {
            {0.f, 0.f, 1.f},
            {0.f, 0.f, 1.f},
            {0.f, 0.f, 1.f},
            {0.f, 0.f, 1.f},
            {1.f, 0.f, 0.f},
            {1.f, 0.f, 0.f},
            {1.f, 0.f, 0.f},
            {1.f, 0.f, 0.f},
            {0.f, 0.f, -1.f},
            {0.f, 0.f, -1.f},
            {0.f, 0.f, -1.f},
            {0.f, 0.f, -1.f},
            {-1.f, 0.f, 0.f},
            {-1.f, 0.f, 0.f},
            {-1.f, 0.f, 0.f},
            {-1.f, 0.f, 0.f},
            {0.f, 1.f, 0.f},
            {0.f, 1.f, 0.f},
            {0.f, 1.f, 0.f},
            {0.f, 1.f, 0.f},
            {0.f, -1.f, 0.f},
            {0.f, -1.f, 0.f},
            {0.f, -1.f, 0.f},
            {0.f, -1.f, 0.f},
        };

        const UV texture_coords[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
        };

        auto indices = std::vector<std::uint32_t>{
            // Each face: 2 triangles (6 indices)
            // Front face
            0, 1, 2, 2, 3, 0,
            // Right face
            4, 5, 6, 6, 7, 4,
            // Back face
            8, 9, 10, 10, 11, 8,
            // Left face
            12, 13, 14, 14, 15, 12,
            // Top face
            16, 17, 18, 18, 19, 16,
            // Bottom face
            20, 21, 22, 22, 23, 20};

        const auto new_item = _loaded_meshes.emplace("cube", LoadedMeshData{vertices(positions, normals, normals, texture_coords), std::move(indices)});

        return {.vertices = new_item.first->second.vertices, .indices = new_item.first->second.indices};
    }
}