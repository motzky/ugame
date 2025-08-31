#ifdef WIN32
#error This code unit is NOT for Windows

#include "window.h"

#include <cstdint>
#include <optional>
#include <print>
#include <queue>
#include <ranges>

#include <GLFW/glfw3.h>

#include "auto_release.h"
#include "error.h"
#include "event.h"
#include "log.h"
#include "opengl.h"
#include "stop_event.h"

#else

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

    void APIENTRY glfw_error_callback(int, const char *description)
    {
        game::log::error(std::string(description).c_str());
    }

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            game::log::info("ESC key pressed");
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        }

        game::log::info("key pressed");
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

        ::glfwSetErrorCallback(glfw_error_callback);
    }
}

namespace game
{
    Window::Window(std::uint32_t width, std::uint32_t height)
        // For non-Windows platforms, you might want to handle this differently
        : _windowHandle{} // Placeholder for non-Windows platforms
    {
        game::ensure(glfwInit(), "failed to initialize GLFW");

        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        _windowHandle = {::glfwCreateWindow(width, height, "Game Window", nullptr, nullptr),
                         ::glfwDestroyWindow};
        game::ensure(_windowHandle, "failed to create window");

        ::glfwSetKeyCallback(_windowHandle, key_callback);

        ::glfwMakeContextCurrent(_windowHandle);

        ::resolve_global_gl_functions();
        ::setup_debug();

        // ::glEnable(GL_CULL_FACE);
        ::glEnable(GL_DEPTH_TEST);
    }
    auto Window::pump_event() -> std::optional<Event>
    {
        ::glfwPollEvents();

        if (::glfwWindowShouldClose(_windowHandle))
        {
            return g_event_queue.emplace(StopEvent{});
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
        ::glfwSwapBuffers(_windowHandle);
    }

}

#endif
