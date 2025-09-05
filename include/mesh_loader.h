#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "resource_loader.h"
#include "string_unordered_map.h"
#include "vertex_data.h"

namespace game
{
    struct MeshData
    {
        std::span<const VertexData> vertices;
        std::span<const std::uint32_t> indices;
    };

    class MeshLoader
    {
    public:
        MeshLoader(ResourceLoader &resource_loader);

        auto load(std::string_view model_file, std::string_view model_name) -> MeshData;
        auto sprite() -> MeshData;
        auto cube() -> MeshData;

    private:
        struct LoadedMeshData
        {
            std::vector<VertexData> vertices;
            std::vector<std::uint32_t> indices;
        };

        StringUnorderedMap<LoadedMeshData> _loaded_meshes;
        ResourceLoader &_resource_loader;
    };
}