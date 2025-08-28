#pragma once

#include <format>
#include <source_location>
#include <print>
#include <string_view>

namespace game::log
{

    enum class Level
    {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    template <Level L, class... Args>
    struct print
    {
    };

    template <Level L, class... Args>
    struct print<L, const char *, Args...>
    {
        print(const char *msg, Args &&...args, std::source_location loc = std::source_location::current())
        {
            auto level = "?";
            if constexpr (L == Level::DEBUG)
            {
                level = "DEBUG";
            }
            else if constexpr (L == Level::INFO)
            {
                level = "INFO ";
            }
            else if constexpr (L == Level::WARN)
            {
                level = "WARN ";
            }
            else if constexpr (L == Level::ERROR)
            {
                level = "ERROR";
            }

            std::println("[{}] ({}:{}) - {}", level, loc.file_name(), loc.line(), std::vformat(msg, std::make_format_args(args...)));
        }
    };

    template <Level L, class... Args>
    print(Args...) -> print<L, Args...>;

    template <class... Args>
    using debug = print<Level::DEBUG, Args...>;

    template <class... Args>
    using info = print<Level::INFO, Args...>;

    template <class... Args>
    using warn = print<Level::WARN, Args...>;

    template <class... Args>
    using error = print<Level::ERROR, Args...>;
}