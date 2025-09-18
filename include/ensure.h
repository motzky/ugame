#pragma once

#include <format>
#include <memory>
#include <string_view>

#include "core/exception.h"
#include "log.h"
#include "utils/auto_release.h"

namespace game
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma waring(disable : 4702)
#endif
    template <class... Args>
    auto expect(bool predicate, std::format_string<Args...> msg, Args &&...args) -> void
    {
        if (!predicate)
        {
            log::error("{}", std::format(msg, std::forward<Args>(args)...));
            log::error("{}", std::stacktrace::current(2));
            std::terminate();
            std::unreachable();
        }
    }
#ifdef _MSC_VER
#pragma warning(pop)
#endif

    template <class... Args>
    auto ensure(bool predicate, std::string_view msg, Args &&...args) -> void
    {
        if (!predicate)
        {
            throw Exception(std::vformat(msg, std::make_format_args(std::forward<Args>(args)...)), 2u);
        }
    }

    template <class T, class... Args>
    auto ensure(AutoRelease<T> &obj, std::string_view msg, Args &&...args) -> void
    {
        ensure(!!obj, msg, std::forward<Args>(args)...);
    }

    template <class T, class D, class... Args>
    auto ensure(std::unique_ptr<T, D> &obj, std::string_view msg, Args &&...args) -> void
    {
        ensure(obj != nullptr, msg, std::forward<Args>(args)...);
    }
}
