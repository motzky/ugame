#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

extern "C"
{
#include <lstate.h>
}

namespace game
{
    struct Vector3;

    class LuaScript
    {
    public:
        LuaScript(std::string_view source);

        auto set_function(const std::string &name) const -> void;

        auto set_argument(std::int64_t value) const -> void;
        auto set_argument(std::string_view value) const -> void;
        auto set_argument(const char *value) const -> void;
        auto set_argument(float value) const -> void;
        auto set_argument(const Vector3 &value) const -> void;
        auto set_argument(bool value) const -> void;

        auto get_result(bool &result) const -> void;
        auto get_result(std::int64_t &result) const -> void;
        auto get_result(float &result) const -> void;
        auto get_result(std::string &result) const -> void;
        auto get_result(Vector3 &result) const -> void;

        auto execute(std::uint32_t num_args, std::uint32_t num_results) const -> void;

        auto to_string() const -> std::string;

    private:
        std::unique_ptr<::lua_State, decltype(&::lua_close)> _lua;
    };

    auto to_string(::lua_State *state) -> std::string;

}