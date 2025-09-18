#pragma once

#include <format>

#include "core/key.h"

namespace game
{

    class KeyEvent
    {

    public:
        KeyEvent(Key key, KeyState state);
        auto key() const -> Key;
        auto state() const -> KeyState;

        auto operator==(const KeyEvent &) const -> bool = default;

    private:
        Key _key;
        KeyState _state;
    };
}

template <>
struct std::formatter<game::KeyEvent>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::KeyEvent &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "KeyEvent {} {}", obj.key(), obj.state());
    }
};
