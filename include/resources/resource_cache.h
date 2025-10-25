#pragma once

#include <ranges>
#include <string_view>
#include <tuple>

#include "sound/sound_data.h"
#include "utils/ensure.h"
#include "utils/string_unordered_map.h"

namespace game
{
    class Mesh;
    class Texture;
    class Material;
    class TextureSampler;

    /**
     * A generic resource cache. Allows you to store string keys against the templated types. Keys must be unique per type.
     */
    template <class... T>
    class ResourceCache
    {
    public:
        /**
         * Insert an object into the cache (of the given type). Undefined behaviour if the object already exists.
         *
         * @param name
         *   Name of object to insert.
         *
         * @param args
         *   Arguments to pass to the constructor of the object.
         */
        template <class U, class... Args>
        auto insert(std::string_view name, Args &&...args) -> U *
        {
            auto &map = std::get<StringUnorderedMap<U>>(_maps);

            expect(!map.contains(name), "{} already exists", name);
            const auto [iter, inserted] = map.emplace(std::make_pair(name, U{std::forward<Args>(args)...}));

            expect(inserted, "insertion failed as key exists");

            return std::addressof(iter->second);
        }

        /**
         * Get an object.
         *
         * @param name
         *   Name of object to get, undefined behaviour if it doesn't exist.
         *
         * @returns
         *   Pointer to requested object.
         */
        template <class U>
        auto get(std::string_view name)
        {
            auto &map = std::get<StringUnorderedMap<U>>(_maps);

            const auto iter = map.find(name);
            expect(iter != std::ranges::end(map), "{} doesn't exist", name);

            return std::addressof(iter->second);
        }

        /**
         * Query, if a name is in cache
         *
         * @param name
         *   Name to query, undefined behaviour if it doesn't exist.
         *
         * @returns
         *   true, if name was found in cache of objects of type U
         */
        template <class U>
        auto contains(std::string_view name)
        {
            auto &map = std::get<StringUnorderedMap<U>>(_maps);
            return map.contains(name);
        }

    private:
        /** Object store for given types. */
        std::tuple<StringUnorderedMap<T>...> _maps;
    };

    // default cache for the game
    using DefaultCache = ResourceCache<Mesh, Material, Texture, TextureSampler, SoundData>;
}