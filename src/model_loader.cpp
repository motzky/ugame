#include "model_loader.h"

#include <cstdint>
#include <ranges>
#include <span>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/Logger.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>

#include "resource_loader.h"
#include "ensure.h"
#include "log.h"
#include "string_unordered_map.h"
#include "vector3.h"
#include "vertex_data.h"

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
    ModelLoader::ModelLoader(ResourceLoader &resource_loader)
        : _resource_loader(resource_loader)
    {
    }

    auto ModelLoader::load(std::string_view model_file, std::string_view model_name) -> ModelData
    {
        const auto loaded = _loaded_models.find(model_name);

        if (loaded != std::ranges::cend(_loaded_models))
        {
            return {.vertices = loaded->second.vertices, .indices = loaded->second.indices};
        }

        auto stream = ::aiGetPredefinedLogStream(::aiDefaultLogStream_STDOUT, NULL);
        ::aiAttachLogStream(&stream);

        const auto model_file_data = _resource_loader.load_binary(model_file);
        log::debug("loaded file {} with {} bytes", model_file, model_file_data.size());

        auto importer = ::Assimp::Importer{};
        const auto *scene = importer.ReadFileFromMemory(model_file_data.data(), model_file_data.size(), ::aiProcess_Triangulate, model_file.data());

        ensure(scene != nullptr, "failed to load model {} from {}", model_file, model_name);
        ensure(!(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE), "failed to load model {} from {}", model_file, model_name);

        const auto loaded_meshes = std::span<::aiMesh *>{scene->mMeshes, scene->mMeshes + scene->mNumMeshes};

        log::debug("found {} meshes", loaded_meshes.size());

        for (const auto *mesh : loaded_meshes)
        {
            log::debug("found mesh {}", mesh->mName.C_Str());

            const auto to_vector3 = [](const ::aiVector3D &v)
            { return Vector3{v.x, v.y, v.z}; };

            const auto verts = std::span<::aiVector3D>{mesh->mVertices, mesh->mVertices + mesh->mNumVertices} |
                               std::views::transform(to_vector3);

            const auto normals = std::span<::aiVector3D>{mesh->mNormals, mesh->mNormals + mesh->mNumVertices} |
                                 std::views::transform(to_vector3);

            auto texture_coords = std::vector<UV>{};
            for (auto i = 0u; i < mesh->mNumFaces; ++i)
            {
                texture_coords.push_back({mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
            }

            auto indices = std::vector<std::uint32_t>{};
            for (auto i = 0u; i < mesh->mNumFaces; ++i)
            {
                const auto &face = mesh->mFaces[i];
                for (auto j = 0u; j < face.mNumIndices; ++j)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            _loaded_models.emplace(mesh->mName.C_Str(), LoadedModelData{vertices(verts, normals, texture_coords), std::move(indices)});
        }

        const auto loaded2 = _loaded_models.find(model_name);

        if (loaded2 != std::ranges::cend(_loaded_models))
        {
            return {.vertices = loaded2->second.vertices, .indices = loaded2->second.indices};
        }

        ensure(false, "failed to load {} from {}. Model not found", model_name, model_file);
        return {};
    }

    auto ModelLoader::cube() -> ModelData
    {
        const auto loaded = _loaded_models.find("cube");

        if (loaded != std::ranges::cend(_loaded_models))
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

        const auto new_item = _loaded_models.emplace("cube", LoadedModelData{vertices(positions, normals, texture_coords), std::move(indices)});

        return {.vertices = new_item.first->second.vertices, .indices = new_item.first->second.indices};
    }
}