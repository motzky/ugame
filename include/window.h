#pragma once

#include <cstdint>
#include <optional>

#ifdef _WIN32
#include <windows.h>
#else
#include <GLFW/glfw3.h>
#endif

#include "events/event.h"
#include "utils/auto_release.h"

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
                auto width() const -> std::uint32_t;
                auto height() const -> std::uint32_t;

                auto show_cursor(bool show) const -> void;

        private:
                AutoRelease<HandleType, nullptr> _windowHandle;
                std::uint32_t _width;
                std::uint32_t _height;
#ifdef _WIN32
                AutoRelease<::HDC> _dc;
                ::WNDCLASSA _wc;
#endif
        };
}