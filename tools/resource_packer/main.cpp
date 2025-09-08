#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <print>
#include <set>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "auto_release.h"
#include "ensure.h"
#include "exception.h"
#include "file.h"
#include "log.h"
#include "tlv/tlv_writer.h"

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
}

auto main(int argc, char **argv) -> int
{
    try
    {
        std::println("resource packer!");

        game::ensure(argc == 3, "usage: ./{} <asset_dir> <out_path>", argv[0]);

        const auto image_extensions = std::set<std::string>{".png", ".jpg"};

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

                auto tex_data = game::TextureData{
                    .name = asset_name,
                    .format = to_texture_format(num_channels),
                    .usage = to_texture_usage(file_name),
                    .width = static_cast<std::uint32_t>(w),
                    .height = static_cast<std::uint32_t>(h),
                    .data = v};

                writer.write(tex_data);
            }
        }
        const auto resource_data = writer.yield();
        // game::File out{argv[2], game::CreationMode::CREATE};
        // out.write(resource_data);
        std::ofstream out{argv[2], std::ios::binary};
        out.write(reinterpret_cast<const char *>(resource_data.data()), resource_data.size());
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