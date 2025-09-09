#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/Logger.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "auto_release.h"
#include "ensure.h"
#include "exception.h"
#include "file.h"
#include "log.h"
#include "math_primitives/vector3.h"
#include "mesh_data.h"
#include "primitives/vertex_data.h"
#include "tlv/tlv_writer.h"
#include "uv.h"

namespace
{
    auto to_texture_format(int num_channels) -> game::TextureFormat
    {
        switch (num_channels)
        {
        case 1:
            return game::TextureFormat::R;
        case 3:
            return game::TextureFormat::RGB;
        case 4:
            return game::TextureFormat::RGBA;
        default:
            throw game::Exception(std::format("unsupported number of channesl: {}", num_channels));
        }
    }

    auto to_texture_usage(std::string_view path) -> game::TextureUsage
    {
        if (path.contains(".data"))
        {
            return game::TextureUsage::DATA;
        }
        if (path.contains(".srgb"))
        {
            return game::TextureUsage::SRGB;
        }

        throw game::Exception(std::format("unsupported usage type: {}", path));
    }

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

auto write_texture(const std::string &path, const std::string &asset_name, const std::string &ext, const std::string &file_name, game::TlvWriter &writer) -> void;
auto write_mesh(const std::string &path, const std::string &asset_name, const std::string &ext, const std::string &file_name, game::TlvWriter &writer) -> void;

auto main(int argc, char **argv) -> int
{
    try
    {
        std::println("resource packer!");

        game::ensure(argc == 3, "usage: ./{} <asset_dir> <out_path>", argv[0]);

        const auto image_extensions = std::set<std::string>{".png", ".jpg"};
        const auto mesh_extensions = std::set<std::string>{".fbx", ".ojb"};

        auto writer = game::TlvWriter{};

        for (const auto &entry : std::filesystem::directory_iterator{argv[1]})
        {
            const auto path = entry.path().string();
            const auto file_name = entry.path().filename().string();

            const auto ext = entry.path().extension().string();
            const auto dot_idx = file_name.find(".");
            const auto asset_name = dot_idx != std::string::npos ? file_name.substr(0, dot_idx) : file_name;

            if (image_extensions.contains(ext))
            {
                write_texture(path, asset_name, ext, file_name, writer);
                continue;
            }
            if (mesh_extensions.contains(ext))
            {
                write_mesh(path, asset_name, ext, file_name, writer);
                continue;
            }
        }
        const auto resource_data = writer.yield();
        game::File out{argv[2], game::CreationMode::CREATE};
        out.write(resource_data);
    }
    catch (const game::Exception &err)
    {
        std::println(std::cerr, "{}", err);
    }
    catch (...)
    {
        std::println(std::cerr, "unknown exception");
    }

    return 0;
}

auto write_texture(const std::string &path, const std::string &asset_name, const std::string &ext, const std::string &file_name, game::TlvWriter &writer) -> void
{
    auto w = int{};
    auto h = int{};
    auto num_channels = int{};

    game::ensure(::stbi_info(path.c_str(), &w, &h, &num_channels) == 1, "failed to get image info");

    auto raw_data = std::unique_ptr<::stbi_uc, decltype(&::stbi_image_free)>{
        ::stbi_load(path.c_str(),
                    &w,
                    &h,
                    &num_channels,
                    0),
        ::stbi_image_free};

    game::ensure(raw_data, "failed to load image data");
    std::println("packing: {} {} {} {} {}", asset_name, ext, w, h, num_channels);

    auto num_bytes = static_cast<std::uint32_t>(w * h * num_channels);
    auto v = std::vector<std::byte>{num_bytes};
    auto s = std::span<const std::byte>{reinterpret_cast<const std::byte *>(raw_data.get()), static_cast<size_t>(num_bytes)};
    v.assign(s.begin(), s.end());

    auto tex_data = game::TextureDescription{
        .name = asset_name,
        .format = to_texture_format(num_channels),
        .usage = to_texture_usage(file_name),
        .width = static_cast<std::uint32_t>(w),
        .height = static_cast<std::uint32_t>(h),
        .data = v};

    writer.write(tex_data);
}

auto write_mesh(const std::string &path, const std::string &asset_name, const std::string &ext, const std::string &file_name, game::TlvWriter &writer) -> void
{
    auto stream = ::aiGetPredefinedLogStream(::aiDefaultLogStream_STDOUT, NULL);
    ::aiAttachLogStream(&stream);

    auto importer = ::Assimp::Importer{};
    const auto *scene = importer.ReadFile(path.c_str(), ::aiProcess_Triangulate | ::aiProcess_FlipUVs | ::aiProcess_CalcTangentSpace);

    game::ensure(scene != nullptr, std::format("failed to load model {} from {}", asset_name, file_name));
    game::ensure(!(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE), "failed to load model {} from {}", asset_name, file_name);

    const auto loaded_meshes = std::span<::aiMesh *>{scene->mMeshes, scene->mMeshes + scene->mNumMeshes};

    for (const auto *mesh : loaded_meshes)
    {
        const auto to_vector3 = [](const ::aiVector3D &v)
        { return game::Vector3{v.x, v.y, v.z}; };

        const auto verts = std::span<::aiVector3D>{mesh->mVertices, mesh->mVertices + mesh->mNumVertices} |
                           std::views::transform(to_vector3);

        const auto normals = std::span<::aiVector3D>{mesh->mNormals, mesh->mNormals + mesh->mNumVertices} |
                             std::views::transform(to_vector3);

        const auto tangents = std::span<::aiVector3D>{mesh->mTangents, mesh->mNumVertices} |
                              std::views::transform(to_vector3);

        auto texture_coords = std::vector<game::UV>{};
        for (auto i = 0u; i < mesh->mNumVertices; ++i)
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
        std::println("packing: {} from {} {} - {} verts, {} indices", mesh->mName.C_Str(), file_name, ext, verts.size(), indices.size());

        const auto vertex_data = vertices(verts, normals, tangents, texture_coords);
        writer.write(mesh->mName.C_Str(), {vertex_data, indices});
    }
}