#include "resource_loader.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "ensure.h"
#include "mapped_file.h"

namespace game
{
    ResourceLoader::ResourceLoader(const std::filesystem::path &root)
        : _root(root)
    {
    }

    auto ResourceLoader::load(std::string_view name) const -> MappedFile
    {
        // return {_root / name, std::ios::in};
        return {_root / name};
    }

}