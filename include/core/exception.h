#pragma once

#include <cstdint>
#include <stacktrace>
#include <stdexcept>
#include <string>
#include <string_view>

namespace game
{
    class Exception
    {
    public:
        Exception(const std::string &what, std::uint32_t skip = 1);

        auto stacktrace() const -> std::string;
        auto to_string() const -> std::string;
        auto what() const -> std::string_view;

    private:
        std::string _what;
        std::stacktrace _trace;
    };

}
