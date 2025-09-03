#pragma once

#include "window.h"

namespace game
{
    class DebugUi
    {
    public:
        DebugUi(Window::HandleType window);
        ~DebugUi();

        auto render() const -> void;

    private:
    };
}