#include "window.h"

#include <stdexcept>
#include <cstdint>

namespace game
{
    Window::Window(std::uint32_t width, std::uint32_t height)
#ifdef _WIN32
        : _windowHandle({}) // Initialize the window handle to nullptr
          ,
          _wc({})
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
#else
        // For non-Windows platforms, you might want to handle this differently
        : _windowHandle{} // Placeholder for non-Windows platforms
    {
        _windowHandle = glfwCreateWindow(width, height, "Game Window", nullptr, nullptr);
        if (!_windowHandle)
        {
            throw std::runtime_error("Failed to create GLFW window");
        }
    }
#endif
}