#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace game
{
    class ResourceLoader
    {
    public:
        ResourceLoader(const std::filesystem::path &root);

        auto load_string(std::string_view name) const -> std::string;
        auto load_binary(std::string_view name) const -> std::vector<std::byte>;

    private:
        std::filesystem::path _root;
    };
}