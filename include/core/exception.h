#pragma once

#include <cstdint>
#include <stacktrace>
#include <stdexcept>
#include <string>

namespace game
{
    class Exception : public std::runtime_error
    {
    public:
        Exception(const std::string &what, std::uint32_t skip = 1);

        auto stacktrace() const -> std::string;
        auto to_string() const -> std::string;

    private:
        std::stacktrace _trace;
    };

}
