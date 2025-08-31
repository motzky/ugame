#pragma once

#include <cstdint>
#include <optional>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <GLFW/glfw3.h>
#endif

#include "auto_release.h"
#include "event.h"

namespace game
{
    class Window
    {
    public:
        Window(std::uint32_t width, std::uint32_t height);
        ~Window() = default;

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        Window(Window &&) noexcept = default;
        Window &operator=(Window &&) = default;

        auto pump_event() -> std::optional<Event>;
        auto swap() -> void;

    private:
#ifdef _WIN32
        AutoRelease<::HWND, nullptr> _windowHandle;
        AutoRelease<::HDC> _dc;
        ::WNDCLASSA _wc;
#else
        AutoRelease<::GLFWwindow *, nullptr> _windowHandle; // Placeholder for non-Windows platforms
#endif
    };
}