#ifndef WIN32
#error This code unit is ONLY for Windows !
#else

#include "window.h"

#include <cstdint>
#include <optional>
#include <print>
#include <queue>
#include <ranges>

#include <Windows.h>

#include "auto_release.h"
#include "error.h"
#include "event.h"
#include "log.h"
#include "opengl.h"
#include "stop_event.h"

namespace
{
    auto g_event_queue = std::queue<game::Event>{};

    void APIENTRY opengl_debug_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei,
        const GLchar *message,
        const void *)
    {
        std::println("{} {} {} {} {}", source, type, id, severity, message);
    }

    template <class T>
    auto resolve_gl_function(T &function, const std::string &name) -> void
    {
        const auto address = ::GL_GET_PROC_ADDRESS(name.c_str());
        game::ensure(address != nullptr, "failed to resolve {}", name);

        function = reinterpret_cast<T>(address);
    }

    auto resolve_global_gl_functions() -> void
    {
#define RESOLVE(TYPE, NAME) resolve_gl_function(NAME, #NAME);
        FOR_OPENGL_FUNCTIONS(RESOLVE);
    }

    auto setup_debug() -> void
    {
        ::glEnable(GL_DEBUG_OUTPUT);
        //::glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        ::glDebugMessageCallback(opengl_debug_callback, nullptr);
    }
}

namespace game
{
    Window::Window(std::uint32_t width, std::uint32_t height)
        : _windowHandle({}), _wc({}), _dc({})
    {
        _wc = {
            .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
            .lpfnWndProc = ::DefWindowProcA, // Default window procedure
            .hInstance = ::GetModuleHandle(nullptr),
            .lpszClassName = "GameWindowClass",
        };

        if (!RegisterClassA(&_wc))
        {
            throw std::runtime_error("Failed to register window class");
        }

        ::RECT rect{
            .left = {},
            .top = {},
            .right = static_cast<int>(width),
            .bottom = static_cast<int>(height)};

        if (::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) == 0)
        {
            throw std::runtime_error("Failed to adjust window rectangle");
        }

        _windowHandle = {::CreateWindowExA(
                             0,
                             _wc.lpszClassName                                                             // Class name, should be registered before this
                             "Game Window",                                                                // Window title
                             WS_OVERLAPPEDWINDOW,                                                          // Window style
                             CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, // Position and size
                             nullptr,                                                                      // Parent window
                             nullptr,                                                                      // Menu
                             _wc.hInstance,                                                                // Instance handle
                             nullptr                                                                       // Additional application data
                             ),
                         ::DestroyWindow}; // Use AutoRelease to manage the window handle

        ::ShowWindow(_windowHandle, SW_SHOW); // Show the window
        ::UpdateWindow(_windowHandle);        // Update the window to reflect changes
    }

    auto Window::pump_event() -> std::optional<Event>
    {
        auto message = ::MSG{};
        while (::PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            ::TranslateMessage(&message);
            ::DispatchMessage(&message);
        }

        if (!std::ranges::empty(g_event_queue))
        {
            const auto event = g_event_queue.front();
            g_event_queue.pop();
            return event;
        }

        return {};
    }

    auto Window::swap() -> void
    {
        ::SwapBuffers(_dc)
    }

}
#endif
