#pragma once

#include <memory>
#include <string_view>

extern "C"
{
#include <lstate.h>
}

namespace game
{
    class LuaScript
    {
    public:
        LuaScript(std::string_view source);

    private:
        std::unique_ptr<::lua_State, decltype(&::lua_close)> _lua;
    };
}