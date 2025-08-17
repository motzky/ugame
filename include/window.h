#pragma once

#include <cstdint>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <GLFW/glfw3.h>
#endif

#include "auto_release.h"

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

    private:
#ifdef _WIN32
        AutoRelease<::HWND, nullptr> _windowHandle;
        ::WNDCLASSA _wc;
#else
        GLFWwindow *_windowHandle; // Placeholder for non-Windows platforms
#endif
    };
}