#pragma once

namespace game
{
    template <class T>
    class PassKey
    {
        friend T;
        PassKey() = default;
    };
}