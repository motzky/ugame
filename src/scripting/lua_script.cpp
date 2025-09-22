#include "scripting/lua_script.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>

extern "C"
{
#include <lauxlib.h>
#include <lstate.h>
#include <lua.h>
#include <lualib.h>
}

#include "utils/ensure.h"

namespace
{
    struct LoadData
    {
        std::string_view source;
        std::uint32_t counter;
    };

    auto load_string_view(::lua_State *, void *data, std::size_t *size) -> const char *
    {
        auto *load_data = reinterpret_cast<LoadData *>(data);

        if (load_data->counter != 0u)
        {
            return nullptr;
        }

        *size = load_data->source.size();
        load_data->counter++;
        return load_data->source.data();
    }
}

namespace game
{
    LuaScript::LuaScript(std::string_view source)
        : _lua(::luaL_newstate(), &::lua_close)
    {
        ensure(_lua, "failed to create lua state");

        ::luaL_openlibs(_lua.get());

        auto load_data = LoadData{.source = source, .counter = 0};

        ensure(
            ::lua_load(_lua.get(), &load_string_view, reinterpret_cast<void *>(&load_data), "lua_error", "t") == LUA_OK,
            "failed to load lua script");

        ensure(::lua_pcall(_lua.get(), 0, 0, 0) == LUA_OK, "failed to execute script");

        ::lua_pop(_lua.get(), 0);
    }
}