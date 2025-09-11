#pragma once

#include <format>

namespace game
{
    enum class MouseButtonState
    {
        UP,
        DOWN
    };

    class MouseButtonEvent
    {

    public:
        MouseButtonEvent(float x, float y, MouseButtonState state);

        auto x() const -> float;
        auto y() const -> float;
        auto state() const -> MouseButtonState;

    private:
        float _x;
        float _y;
        MouseButtonState _state;
    };
}

template <>
struct std::formatter<game::MouseButtonState>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::MouseButtonState &obj, std::format_context &ctx) const
    {
        switch (obj)
        {
            using enum game::MouseButtonState;
        case UP:
            return std::format_to(ctx.out(), "UP");
        case DOWN:
            return std::format_to(ctx.out(), "DOWN");
        }
    }
};

template <>
struct std::formatter<game::MouseButtonEvent>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::MouseButtonEvent &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "MouseButtonEvent: {} @ {} {}", obj.state(), obj.x(), obj.y());
    }
};
