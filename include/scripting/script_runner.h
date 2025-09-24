#pragma once

#include <string>

#include "scripting/lua_script.h"

namespace game
{
    namespace impl
    {
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

        template <class... Args>
        auto execute(const std::string &function_name, Args &&...args) const -> void
        {
            const auto set_args = [this](Args &&...args)
            { (_script.set_argument(args), ...); };

            _script.set_function(function_name);
            set_args(std::forward<Args>(args)...);
            _script.execute(sizeof...(args), 0u);
        }

    private:
        LuaScript &_script;
    };

}
