#include "scripting/lua_script.h"

#include <memory>

extern "C"
{
#include <lauxlib.h>
#include <lstate.h>
#include <lua.h>
#include <lualib.h>
}

namespace game
{
    LuaScript::LuaScript()
        : _lua(::luaL_newstate(), &::lua_close)
    {
    }
}