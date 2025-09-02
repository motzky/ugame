#ifdef WIN32
#error This code unit is NOT for Windows
#else

#include "window.h"

#include <cstdint>
#include <optional>
#include <print>
#include <queue>
#include <ranges>

#include <GLFW/glfw3.h>

#include "auto_release.h"
#include "ensure.h"
#include "event.h"
#include "key.h"
#include "key_event.h"
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
        if (type == GL_DEBUG_TYPE_ERROR)
        {
            game::ensure(false, "{} {} {} {} {}", source, type, id, severity, message);
        }
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            game::log::error("{} - {}: {} from {}", id, type, message, source);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            game::log::warn("{} - {}: {} from {}", id, type, message, source);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            game::log::info("{} - {}: {} from {}", id, type, message, source);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            game::log::debug("{} - {}: {} from {}", id, type, message, source);
            break;
        }
    }

    void APIENTRY glfw_error_callback(int, const char *description)
    {
        game::log::error("{}", description);
    }

    // void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    void key_callback(GLFWwindow *window, int key, int, int action, int)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        }

        if (action == GLFW_PRESS)
        {
            g_event_queue.emplace(game::KeyEvent{static_cast<game::Key>(key), game::KeyState::DOWN});
            return;
        }
        if (action == GLFW_RELEASE)
        {
            g_event_queue.emplace(game::KeyEvent{static_cast<game::Key>(key), game::KeyState::UP});
            return;
        }
    }

    void mouse_callback(GLFWwindow * /*window*/, double xpos, double ypos)
    {
        static auto last_x = float{};
        static auto last_y = float{};

        auto xf = static_cast<float>(xpos);
        auto yf = static_cast<float>(ypos);

        // game::log::debug("Mouse pos: {} {}", xpos, ypos);

        auto dx = last_x - xf;
        auto dy = last_y - yf;

        // game::log::debug("Mouse delta: {} {}", dx, dy);

        g_event_queue.emplace(game::MouseEvent{dx, dy});

        last_x = xf;
        last_y = yf;
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
        // ::glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
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
        if (::glfwRawMouseMotionSupported())
        {
            ::glfwSetInputMode(_windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ::glfwSetInputMode(_windowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
        else
        {
            ::glfwSetInputMode(_windowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        ::glfwSetCursorPosCallback(_windowHandle, mouse_callback);

        // TODO
        // ::glfwSetMouseButtonCallback(_windowHandle, mouse_button_callback);
        // ::glfwSetScrollCallback(_windowHandle, scroll_callback);

        ::glfwMakeContextCurrent(_windowHandle);

        ::resolve_global_gl_functions();
        ::setup_debug();

        ::glEnable(GL_CULL_FACE);
        ::glEnable(GL_DEPTH_TEST);

        auto vendor_str = ::glGetString(GL_VENDOR);
        game::log::info("Current render device vendor: {}", reinterpret_cast<const char *>(vendor_str));

        auto renderer_str = ::glGetString(GL_RENDERER);
        game::log::info("Current render device: {}", reinterpret_cast<const char *>(renderer_str));
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
