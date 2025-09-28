#include "scripting/lua_script.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>

extern "C"
{
#include <lauxlib.h>
#include <lstate.h>
#include <lua.h>
#include <lualib.h>
}

#include "math/vector3.h"
#include "scripting/vector3_interop.h"
#include "utils/ensure.h"
#include "utils/exception.h"

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
        lua_register(_lua.get(), "Vector3", &vector3_constructor);
        lua_register(_lua.get(), "distance", &vector3_distance);

        ::luaL_newmetatable(_lua.get(), "Vector3");

        ::lua_pushcfunction(_lua.get(), &vector3_add);
        ::lua_setfield(_lua.get(), -2, "__add");
        ::lua_pushcfunction(_lua.get(), &vector3_sub);
        ::lua_setfield(_lua.get(), -2, "__sub");
        ::lua_pushcfunction(_lua.get(), &vector3_mul);
        ::lua_setfield(_lua.get(), -2, "__mul");
        ::lua_pushcfunction(_lua.get(), &vector3_unm);
        ::lua_setfield(_lua.get(), -2, "__unm");
        ::lua_pushcfunction(_lua.get(), &vector3_eq);
        ::lua_setfield(_lua.get(), -2, "__eq");
        ::lua_pushcfunction(_lua.get(), &vector3_tostring);
        ::lua_setfield(_lua.get(), -2, "__tostring");

        ::lua_getmetatable(_lua.get(), -1);

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

    auto LuaScript::set_argument(const char *value) const -> void
    {
        set_argument(std::string_view{value});
    }

    auto LuaScript::set_argument(std::int64_t value) const -> void
    {
        ::lua_pushinteger(_lua.get(), value);
    }

    auto LuaScript::set_argument(float value) const -> void
    {
        ::lua_pushnumber(_lua.get(), value);
    }

    auto LuaScript::set_argument(bool value) const -> void
    {
        ::lua_pushboolean(_lua.get(), value);
    }

    auto LuaScript::set_argument(const Vector3 &value) const -> void
    {
        set_argument(value.x);
        set_argument(value.y);
        set_argument(value.z);

        vector3_constructor(_lua.get());
    }

    auto LuaScript::get_result(bool &result) const -> void
    {
        ensure(::lua_gettop(_lua.get()) != 0, "no reuslt to get\n{}", *this);
        ensure(lua_isboolean(_lua.get(), -1) == 1, "result not a boolean\n{}", *this);
        result = ::lua_toboolean(_lua.get(), -1);
        ::lua_pop(_lua.get(), 1);
    }

    auto LuaScript::get_result(std::int64_t &result) const -> void
    {
        ensure(::lua_gettop(_lua.get()) != 0, "no reuslt to get\n{}", *this);
        ensure(::lua_isinteger(_lua.get(), -1) == 1, "result not an integer\n{}", *this);
        result = ::lua_tointeger(_lua.get(), -1);
        ::lua_pop(_lua.get(), 1);
    }

    auto LuaScript::get_result(float &result) const -> void
    {
        ensure(::lua_gettop(_lua.get()) != 0, "no reuslt to get\n{}", *this);
        ensure(::lua_isnumber(_lua.get(), -1) == 1, "result not a float\n{}", *this);
        result = static_cast<float>(::lua_tonumber(_lua.get(), -1));
        ::lua_pop(_lua.get(), 1);
    }

    auto LuaScript::get_result(std::string &result) const -> void
    {
        ensure(::lua_gettop(_lua.get()) != 0, "no reuslt to get\n{}", *this);
        ensure(::lua_isstring(_lua.get(), -1) == 1, "result not a string\n{}", *this);
        result = ::lua_tostring(_lua.get(), -1);
        ::lua_pop(_lua.get(), 1);
    }

    auto LuaScript::get_result(Vector3 &result) const -> void
    {
        ensure(::lua_gettop(_lua.get()) != 0, "no reuslt to get:\n{}", *this);

        ensure(lua_istable(_lua.get(), -1), "result not a table:\n{}", *this);

        ensure(::lua_getfield(_lua.get(), -1, "x") == LUA_TNUMBER, "could not get x field\n{}", *this);
        result.x = static_cast<float>(::lua_tonumber(_lua.get(), -1));
        ::lua_pop(_lua.get(), 1);

        ensure(::lua_getfield(_lua.get(), -1, "y") == LUA_TNUMBER, "could not get y field\n{}", *this);
        result.y = static_cast<float>(::lua_tonumber(_lua.get(), -1));
        ::lua_pop(_lua.get(), 1);

        ensure(::lua_getfield(_lua.get(), -1, "z") == LUA_TNUMBER, "could not get z field\n{}", *this);
        result.z = static_cast<float>(::lua_tonumber(_lua.get(), -1));
        ::lua_pop(_lua.get(), 1);

        ::lua_pop(_lua.get(), 1);
    }

    auto LuaScript::execute(std::uint32_t num_args, std::uint32_t num_results) const -> void
    {
        ensure(::lua_gettop(_lua.get()) >= static_cast<int>(num_args), "arg count mismatch {}", num_args);
        if (::lua_pcall(_lua.get(), num_args, num_results, 0) != LUA_OK)
        {
            const auto res = ::lua_tostring(_lua.get(), -1);

            ::lua_pop(_lua.get(), 1);

            throw Exception("failed to execute ({})", res);
        }
    }

    auto LuaScript::to_string() const -> std::string
    {
        return game::to_string(_lua.get());
    }

    auto to_string(::lua_State *state) -> std::string
    {
        auto ss = std::stringstream{};

        const auto stack_size = ::lua_gettop(state);
        if (stack_size == 0)
        {
            return "<empty stack>";
        }

        for (const auto index : std::views::iota(1, stack_size + 1) | std::views::reverse)
        {
            const auto type = ::lua_type(state, index);
            switch (type)
            {
            case LUA_TNIL:
                ss << "LUA_TNIL";
                break;
            case LUA_TNUMBER:
            {
                if (::lua_isinteger(state, index) == 1)
                {
                    ss << std::format("LUA_NUMBER (int) {}", ::lua_tointeger(state, index));
                }
                else
                {
                    ss << std::format("LUA_NUMBER (float) {}", ::lua_tonumber(state, index));
                }
                break;
            }
            break;
            case LUA_TBOOLEAN:
                ss << std::format("LUA_TBOOLEAN {}", ::lua_toboolean(state, index));
                break;
            case LUA_TSTRING:
                ss << std::format("LUA_TSTRING '{}'", ::lua_tostring(state, index));
                break;
            case LUA_TTABLE:
                ss << "LUA_TTABLE";
                break;
            case LUA_TFUNCTION:
                ss << "LUA_TFUNCTION";
                break;
            case LUA_TUSERDATA:
                ss << "LUA_TUSERDATA";
                break;
            case LUA_TTHREAD:
                ss << "LUA_TTHREAD";
                break;
            case LUA_TLIGHTUSERDATA:
                ss << "LUA_TLIGHTUSERDATA";
                break;
            default:
                ss << "unknown";
                break;
            }

            ss << "\n";
        }

        return ss.str();
    }
}