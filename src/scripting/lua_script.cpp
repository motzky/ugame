#include "scripting/lua_script.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

extern "C"
{
#include <lauxlib.h>
#include <lstate.h>
#include <lua.h>
#include <lualib.h>
}

#include "core/exception.h"
#include "math/vector3.h"
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
    }

    auto LuaScript::set_function(const std::string &name) const -> void
    {
        const auto ret_type = ::lua_getglobal(_lua.get(), name.c_str());
        ensure(ret_type == LUA_TFUNCTION, "not a lua function: {}", name);
    }

    auto LuaScript::set_argument(std::string_view value) const -> void
    {
        ::lua_pushlstring(_lua.get(), value.data(), value.size());
    }

    auto LuaScript::set_argument(std::int64_t value) const -> void
    {
        ::lua_pushinteger(_lua.get(), value);
    }

    auto LuaScript::set_argument(float value) const -> void
    {
        ::lua_pushnumber(_lua.get(), value);
    }

    auto LuaScript::set_argument(const Vector3 &value) const -> void
    {
        set_argument(value.x);
        set_argument(value.y);
        set_argument(value.z);

        // vector3_constructor(lua_.get());
    }

    auto LuaScript::get_result(std::int64_t &result) const -> void
    {
        ensure(::lua_gettop(_lua.get()) != 0, "no reuslt to get");
        ensure(::lua_isinteger(_lua.get(), -1) == 1, "result not an integer");
        result = ::lua_tointeger(_lua.get(), -1);
        ::lua_pop(_lua.get(), 1);
    }

    auto LuaScript::get_result(float &result) const -> void
    {
        ensure(::lua_gettop(_lua.get()) != 0, "no reuslt to get");
        ensure(::lua_isnumber(_lua.get(), -1) == 1, "result not a float");
        result = static_cast<float>(::lua_tonumber(_lua.get(), -1));
        ::lua_pop(_lua.get(), 1);
    }

    auto LuaScript::get_result(std::string &result) const -> void
    {
        ensure(::lua_gettop(_lua.get()) != 0, "no reuslt to get");
        ensure(::lua_isstring(_lua.get(), -1) == 1, "result not a string");
        result = ::lua_tostring(_lua.get(), -1);
        ::lua_pop(_lua.get(), 1);
    }

    auto LuaScript::execute(std::uint32_t num_args, std::uint32_t num_results) const -> void
    {
        ensure(::lua_gettop(_lua.get()) >= static_cast<int>(num_args), "arg count mismatch {}", num_args);
        if (::lua_pcall(_lua.get(), num_args, num_results, 0) != LUA_OK)
        {
            const auto res = ::lua_tostring(_lua.get(), -1);

            ::lua_pop(_lua.get(), 1);

            throw Exception(std::format("failed to execture ({})", res));
        }
    }

}