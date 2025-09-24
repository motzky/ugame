#pragma once

#include <string>
#include <tuple>

#include "scripting/lua_script.h"

namespace game
{
    namespace impl
    {
        template <std::size_t I = 0u, class... R>
        auto get_result(LuaScript &script, std::tuple<R...> &result_tuple) -> void
        {
            script.get_result(std::get<sizeof...(R) - I - 1>(result_tuple));

            if constexpr (I != sizeof...(R) - 1)
            {
                get_result<I + 1>(script, result_tuple);
            }
        }
    }

    class ScriptRunner
    {
    public:
        ScriptRunner(LuaScript &script)
            : _script(script)
        {
        }

        ScriptRunner(const ScriptRunner &) = delete;
        ScriptRunner &operator=(const ScriptRunner) = delete;
        ScriptRunner(ScriptRunner &&) = delete;
        ScriptRunner &operator=(ScriptRunner &&) = delete;

        template <class... R, class... Args>
        auto execute(const std::string &function_name, Args &&...args) const
        {
            const auto set_args = [this](Args &&...args)
            {
                (_script.set_argument(args), ...);
            };

            _script.set_function(function_name);
            set_args(std::forward<Args>(args)...);

            _script.execute(sizeof...(args), sizeof...(R));

            // note: "else if" is needed for auto return type deduction,
            //       if we remove the else as we would in normal if() {return;}
            //       compiler will complain about inconsistent return type deduction (first void and then std::tuple<>)
            if constexpr (sizeof...(R) == 0)
            {
                return;
            }
            else if constexpr (sizeof...(R) == 1)
            {
                auto res = std::tuple<R...>{};
                _script.get_result(std::get<0>(res));
                return std::get<0>(res);
            }
            else
            {
                auto res = std::tuple<R...>{};
                impl::get_result(_script, res);
                return res;
            }
        }

    private:
        LuaScript &_script;
    };

}
