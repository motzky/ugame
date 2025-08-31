#pragma once

#include <format>
#include <source_location>
#include <print>

namespace game::log
{
    enum class Level
    {
        DEBUG,
        INFO,
        WARN,
#ifndef WIN32
        ERROR
#else
        // We cannot use ERROR, if we included <windows.h> since that has a define for ERROR which then leads to compile errors here
        ERR
#endif
    };

    template <Level L, class... Args>
    struct Print
    {
    };

    template <Level L, class... Args>
    struct Print<L, const char *, Args...>
    {
        Print(const char *msg, Args &&...args, std::source_location loc = std::source_location::current())
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
#ifndef WIN32
            else if constexpr (L == Level::ERROR)
#else
            else if constexpr (L == Level::ERR)
#endif
            {
                level = "ERROR";
            }

            std::println("[{}] ({}:{}) - {}", level, loc.file_name(), loc.line(), std::vformat(msg, std::make_format_args(args...)));
        }
    };

    template <Level L, class... Args>
    Print(Args &&...) -> Print<L, Args...>;

    template <class... Args>
    using debug = Print<Level::DEBUG, const char *, Args...>;

    template <class... Args>
    using info = Print<Level::INFO, const char *, Args...>;

    template <class... Args>
    using warn = Print<Level::WARN, const char *, Args...>;

    template <class... Args>
#ifndef WIN32
    using error = Print<Level::ERROR, const char *, Args...>;
#else
    using error = Print<Level::ERR, const char *, Args...>;
#endif
}