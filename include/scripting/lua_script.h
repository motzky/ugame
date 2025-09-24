#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

extern "C"
{
#include <lstate.h>
}

#include "math/vector3.h"

namespace game
{
    class LuaScript
    {
    public:
        LuaScript(std::string_view source);

        auto set_function(const std::string &name) const -> void;

        auto set_argument(std::string_view value) const -> void;
        auto set_argument(std::int64_t value) const -> void;
        auto set_argument(float value) const -> void;
        auto set_argument(const Vector3 &value) const -> void;

        auto get_result(std::int64_t &result) const -> void;

        auto execute(std::uint32_t num_args, std::uint32_t num_results) const -> void;

    private:
        std::unique_ptr<::lua_State, decltype(&::lua_close)> _lua;
    };
}