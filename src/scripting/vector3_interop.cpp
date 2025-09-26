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
}

namespace game
{
    auto vector3_constructor(::lua_State *state) -> int
    {
        ensure(::lua_isnumber(state, -3) == 1, "stack index 1 is not number: {}", state);
        const auto x = static_cast<float>(::lua_tonumber(state, -3));
        ensure(::lua_isnumber(state, -2) == 1, "stack index 2 is not number: {}", state);
        const auto y = static_cast<float>(::lua_tonumber(state, -2));
        ensure(::lua_isnumber(state, -1) == 1, "stack index 3 is not number: {}", state);
        const auto z = static_cast<float>(::lua_tonumber(state, -1));

        ::lua_pop(state, 3);

        push_vector3(state, {x, y, z});

        return 1;
    }

    auto vector3_add(::lua_State * /*state*/) -> int
    {
        return 1;
    }

    auto vector3_sub(::lua_State * /*state*/) -> int
    {
        return 1;
    }

    auto vector3_mul(::lua_State * /*state*/) -> int
    {
        return 1;
    }

    auto vector3_unm(::lua_State * /*state*/) -> int
    {
        return 1;
    }

    auto vector3_eq(::lua_State * /*state*/) -> int
    {
        return 1;
    }

    auto vector3_distance(::lua_State * /*state*/) -> int
    {
        return 1;
    }

    // auto vector3_tostring(::lua_State */*state*/) -> int;
}