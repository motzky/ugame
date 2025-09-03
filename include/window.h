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
#ifdef _WIN32
        typedef ::HWND HandleType;
#else
        typedef ::GLFWwindow *HandleType;
#endif

        Window(std::uint32_t width, std::uint32_t height);
        ~Window() = default;

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        Window(Window &&) noexcept = default;
        Window &operator=(Window &&) = default;

        auto pump_event() -> std::optional<Event>;
        auto swap() -> void;

        auto native_handle() const -> HandleType;

    private:
        AutoRelease<HandleType, nullptr> _windowHandle;
#ifdef _WIN32
        AutoRelease<::HDC> _dc;
        ::WNDCLASSA _wc;
#endif
    };
}