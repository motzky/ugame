#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "file.h"

namespace game
{
    class ResourceLoader
    {
    public:
        ResourceLoader(const std::filesystem::path &root);

        auto load(std::string_view name) const -> File;

    private:
        std::filesystem::path _root;
    };
}