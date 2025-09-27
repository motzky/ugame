#include "core/exception.h"

#include <format>
#include <iostream>

namespace game
{
    Exception::Exception(const std::string &what, std::uint32_t skip)
        : _what(std::move(what)), _trace(std::stacktrace::current(skip))

    {
    }

    auto Exception::stacktrace() const -> std::string
    {
        return std::to_string(_trace);
    }

    auto Exception::what() const -> std::string_view
    {
        return _what;
    }

    auto Exception::to_string() const -> std::string
    {
        return std::format("{}\n{}", what(), stacktrace());
    }

}
