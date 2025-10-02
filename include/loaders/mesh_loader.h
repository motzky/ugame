#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "graphics/mesh_data.h"
#include "graphics/vertex_data.h"
#include "resources/resource_loader.h"
#include "utils/string_unordered_map.h"

namespace game
{
    class MeshLoader
    {
    public:
        auto sprite() -> MeshData;
        auto cube() -> MeshData;

    private:
        struct LoadedMeshData
        {
            std::vector<VertexData> vertices;
            std::vector<std::uint32_t> indices;
        };

        StringUnorderedMap<LoadedMeshData> _loaded_meshes;
    };
}