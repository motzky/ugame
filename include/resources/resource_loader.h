#pragma once

#include <filesystem>
#include <string_view>

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