#include "scripting/vector3_interop.h"

extern "C"
{
#include <lauxlib.h>
#include <lstate.h>
#include <lua.h>
}

#include "scripting/lua_script.h"
#include "utils/formatter.h"

#include "math/vector3.h"
#include "utils/ensure.h"

namespace
{
    auto push_vector3(::lua_State *state, const game::Vector3 &v) -> void
    {
        ::lua_newtable(state);
        ::luaL_setmetatable(state, "Vector3");

        ::lua_pushnumber(state, v.x);
        ::lua_setfield(state, -2, "x");

        ::lua_pushnumber(state, v.y);
        ::lua_setfield(state, -2, "y");

        ::lua_pushnumber(state, v.z);
        ::lua_setfield(state, -2, "z");
    }

    auto pop_vector3(::lua_State *state) -> game::Vector3
    {
        game::ensure(lua_istable(state, -1), "result not a table:\n{}", state);

        auto result = game::Vector3{};

        game::ensure(::lua_getfield(state, -1, "x") == LUA_TNUMBER, "could not get x field\n{}", state);
        result.x = static_cast<float>(::lua_tonumber(state, -1));
        ::lua_pop(state, 1);

        game::ensure(::lua_getfield(state, -1, "y") == LUA_TNUMBER, "could not get y field\n{}", state);
        result.y = static_cast<float>(::lua_tonumber(state, -1));
        ::lua_pop(state, 1);

        game::ensure(::lua_getfield(state, -1, "z") == LUA_TNUMBER, "could not get z field\n{}", state);
        result.z = static_cast<float>(::lua_tonumber(state, -1));
        ::lua_pop(state, 1);

        ::lua_pop(state, 1);

        return result;
    }
}

namespace game
{
    auto vector3_constructor(::lua_State *state) -> int
    {
        ensure(::lua_isnumber(state, -3) == 1, "stack index 1 is not number:\n{}", state);
        const auto x = static_cast<float>(::lua_tonumber(state, -3));
        ensure(::lua_isnumber(state, -2) == 1, "stack index 2 is not number:\n{}", state);
        const auto y = static_cast<float>(::lua_tonumber(state, -2));
        ensure(::lua_isnumber(state, -1) == 1, "stack index 3 is not number:\n{}", state);
        const auto z = static_cast<float>(::lua_tonumber(state, -1));

        ::lua_pop(state, 3);

        push_vector3(state, {x, y, z});

        return 1;
    }

    auto vector3_add(::lua_State *state) -> int
    {
        ensure(::lua_gettop(state) >= 2, "not enough arguments on stack;\n{}", state);

        const auto vec2 = pop_vector3(state);
        const auto vec1 = pop_vector3(state);

        push_vector3(state, vec1 + vec2);

        return 1;
    }

    auto vector3_sub(::lua_State *state) -> int
    {
        ensure(::lua_gettop(state) >= 2, "not enough arguments on stack;\n{}", state);

        const auto vec2 = pop_vector3(state);
        const auto vec1 = pop_vector3(state);

        push_vector3(state, vec1 - vec2);

        return 1;
    }

    auto vector3_mul(::lua_State *state) -> int
    {
        ensure(::lua_gettop(state) >= 2, "not enough arguments on stack;\n{}", state);

        ensure(::lua_isnumber(state, -1) == 1, "result not a float\n{}", state);
        const auto scalar = static_cast<float>(::lua_tonumber(state, -1));
        ::lua_pop(state, 1);

        const auto vec1 = pop_vector3(state);

        push_vector3(state, vec1 * scalar);

        return 1;
    }

    auto vector3_unm(::lua_State *state) -> int
    {
        ensure(::lua_gettop(state) >= 1, "not enough arguments on stack;\n{}", state);

        const auto vec1 = pop_vector3(state);

        push_vector3(state, -vec1);

        return 1;
    }

    auto vector3_eq(::lua_State *state) -> int
    {
        ensure(::lua_gettop(state) >= 2, "not enough arguments on stack;\n{}", state);

        const auto vec2 = pop_vector3(state);
        const auto vec1 = pop_vector3(state);

        ::lua_pushboolean(state, vec1 == vec2);

        return 1;
    }

    auto vector3_distance(::lua_State *state) -> int
    {
        ensure(::lua_gettop(state) >= 2, "not enough arguments on stack;\n{}", state);

        const auto vec2 = pop_vector3(state);
        const auto vec1 = pop_vector3(state);

        ::lua_pushnumber(state, game::Vector3::distance(vec1, vec2));

        return 1;
    }

    auto vector3_tostring(::lua_State *state) -> int
    {
        ensure(::lua_gettop(state) >= 1, "not enough arguments on stack;\n{}", state);

        const auto vec1 = pop_vector3(state);

        ::lua_pushstring(state, vec1.to_string().c_str());

        return 1;
    }
}