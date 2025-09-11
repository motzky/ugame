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

    private:
        std::stacktrace _trace;
    };

}

template <>
struct std::formatter<game::Exception>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::Exception &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "{}\n{}", obj.what(), obj.stacktrace());
    }
};