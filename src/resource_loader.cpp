#include "resource_loader.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "ensure.h"

namespace
{

    auto load(const std::filesystem::path &path, auto open_mode) -> std::string
    {
        std::ifstream file{path, open_mode};
        game::ensure(!!file, "failed to open file {}");

        auto ss = std::stringstream{};
        ss << file.rdbuf();

        return ss.str();
    }
}

namespace game
{
    ResourceLoader::ResourceLoader(const std::filesystem::path &root)
        : _root(root)
    {
    }
    auto ResourceLoader::load_string(std::string_view name) const -> std::string
    {
        return load(_root / name, std::ios::in);
    }

    auto ResourceLoader::load_binary(std::string_view name) const -> std::vector<std::byte>
    {
        const auto data = load(_root / name, std::ios::in | std::ios::binary);

        return data //
               | std::views::transform([](auto b)
                                       { return static_cast<std::byte>(b); }) //
               | std::ranges::to<std::vector>();
    }
}