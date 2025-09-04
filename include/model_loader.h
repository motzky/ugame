#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "vertex_data.h"
#include "string_unordered_map.h"

namespace game
{
    struct ModelData
    {
        std::span<const VertexData> vertices;
        std::span<const std::uint32_t> indices;
    };

    class ModelLoader
    {
    public:
        auto cube() -> ModelData;

    private:
        struct LoadedModelData
        {
            std::vector<VertexData> vertices;
            std::vector<std::uint32_t> indices;
        };

        StringUnorderedMap<LoadedModelData> _loaded_models;
    };
}