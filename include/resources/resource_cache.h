#pragma once

#include <ranges>
#include <string_view>
#include <tuple>

#include "ensure.h"
#include "utils/string_unordered_map.h"

namespace game
{
    class Mesh;
    class Texture;
    class Material;
    class TextureSampler;

    template <class... T>
    class ResourceCache
    {
    public:
        template <class U, class... Args>
        auto insert(std::string_view name, Args &&...args) -> U *
        {
            auto &map = std::get<StringUnorderedMap<U>>(_maps);

            expect(!map.contains(name), "{} already exists", name);
            const auto [iter, inserted] = map.emplace(std::make_pair(name, U{std::forward<Args>(args)...}));

            expect(inserted, "insertion failed as key exists");

            return std::addressof(iter->second);
        }

        template <class U>
        auto get(std::string_view name) -> U *
        {
            auto &map = std::get<StringUnorderedMap<U>>(_maps);
            const auto iter = map.find(name);
            expect(iter != std::ranges::end(map), "{} doesn;t exists", name);
            return std::addressof(iter->second);
        }

    private:
        std::tuple<StringUnorderedMap<T>...> _maps;
    };

    using DefaultCache = ResourceCache<Mesh, Material, Texture, TextureSampler>;
}