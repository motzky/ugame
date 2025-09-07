#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "mapped_file.h"

namespace game
{
    class ResourceLoader
    {
    public:
        ResourceLoader(const std::filesystem::path &root);

        auto load(std::string_view name) const -> MappedFile;

    private:
        std::filesystem::path _root;
    };
}