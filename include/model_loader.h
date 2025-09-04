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
    struct ModelData
    {
        std::span<const VertexData> vertices;
        std::span<const std::uint32_t> indices;
    };

    class ModelLoader
    {
    public:
        ModelLoader(ResourceLoader &resource_loader);

        auto load(std::string_view model_file, std::string_view model_name) -> ModelData;

        auto cube() -> ModelData;

    private:
        struct LoadedModelData
        {
            std::vector<VertexData> vertices;
            std::vector<std::uint32_t> indices;
        };

        StringUnorderedMap<LoadedModelData> _loaded_models;
        ResourceLoader &_resource_loader;
    };
}