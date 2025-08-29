#include "window.h"

#include <cstdint>
#include <print>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include "auto_release.h"
#include "error.h"
#include "opengl.h"

namespace
{
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
        game::ensure(glfwInit(), "failed to initialize GLFW");

        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        _windowHandle = {::glfwCreateWindow(width, height, "Game Window", nullptr, nullptr),
                         ::glfwDestroyWindow};
        game::ensure(_windowHandle, "failed to create window");

        ::glfwMakeContextCurrent(_windowHandle);

        ::resolve_global_gl_functions();
        ::setup_debug();

        ::glEnable(GL_CULL_FACE);
    }
#endif

    auto Window::windowShouldClose() -> bool
    {
#ifdef WIN32
        return true;
#else
        ::glfwPollEvents();

        return ::glfwWindowShouldClose(_windowHandle);
#endif
    }

    auto Window::swapBuffers() -> void
    {
#ifdef WIN32
        ::SwapBuffers(_windowHandle)
#else
        ::glfwSwapBuffers(_windowHandle);
#endif
    }

}