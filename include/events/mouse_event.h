#pragma once

#include <format>

namespace game
{

    class MouseEvent
    {

    public:
        MouseEvent(float delta_x, float delta_y);

        auto delta_x() const -> float;
        auto delta_y() const -> float;

    private:
        float _delta_x;
        float _delta_y;
    };
}

template <>
struct std::formatter<game::MouseEvent>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::MouseEvent &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "MouseEvent {} {}", obj.delta_x(), obj.delta_y());
    }
};
