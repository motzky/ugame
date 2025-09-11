#include "loaders/resource_loader.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "ensure.h"
#include "file.h"

namespace game
{
    ResourceLoader::ResourceLoader(const std::filesystem::path &root)
        : _root(root)
    {
    }

    auto ResourceLoader::load(std::string_view name) const -> File
    {
        // return {_root / name, std::ios::in};
        return {_root / name};
    }

}