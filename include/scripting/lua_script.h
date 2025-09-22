#pragma once

#include <memory>

extern "C"
{
#include <lstate.h>
}

namespace game
{
    class LuaScript
    {
    public:
        LuaScript();

    private:
        std::unique_ptr<::lua_State, decltype(&::lua_close)> _lua;
    };
}