#ifdef WIN32
#error This code unit is NOT for Windows
#endif

#include "window.h"

#include <cstdint>
#include <optional>
#include <print>
#include <queue>
#include <ranges>

#include <GLFW/glfw3.h>

#include "auto_release.h"
#include "ensure.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "key.h"
#include "log.h"
#include "opengl.h"
#include "stop_event.h"

namespace
{
    auto g_event_queue = std::queue<game::Event>{};
    auto g_process_mouse_events = false;

    void APIENTRY opengl_debug_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei,
        const GLchar *message,
        const void *)
    {
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

    auto key_callback(GLFWwindow *window, int key, int, int action, int) -> void
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

    auto mouse_button_callback(GLFWwindow *window, int button, int action, int /*mods*/) -> void
    {
        if (!g_process_mouse_events)
        {
            return;
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            auto state = action == GLFW_PRESS ? game::MouseButtonState::DOWN : game::MouseButtonState::UP;

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            g_event_queue.emplace(game::MouseButtonEvent{static_cast<float>(xpos), static_cast<float>(ypos), state});
        }
    }

    auto mouse_cursor_entered_left_callback(GLFWwindow *, int entered) -> void
    {
        if (entered)
        {
            g_process_mouse_events = true;
        }
        else
        {
            g_process_mouse_events = false;
        }
    }

    auto mouse_callback(GLFWwindow * /*window*/, double xpos, double ypos) -> void
    {
        if (!g_process_mouse_events)
        {
            return;
        }
        static auto last_x = float{};
        static auto last_y = float{};

        auto xf = static_cast<float>(xpos);
        auto yf = static_cast<float>(ypos);

        auto dx = last_x - xf;
        auto dy = last_y - yf;

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
        : _windowHandle{}, _width(width), _height(height)
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
            // ::glfwSetInputMode(_windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ::glfwSetInputMode(_windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            ::glfwSetInputMode(_windowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
        else
        {
            ::glfwSetInputMode(_windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        ::glfwSetCursorEnterCallback(_windowHandle, mouse_cursor_entered_left_callback);
        ::glfwSetCursorPosCallback(_windowHandle, mouse_callback);
        ::glfwSetMouseButtonCallback(_windowHandle, mouse_button_callback);
        // TODO
        // ::glfwSetScrollCallback(_windowHandle, scroll_callback);

        ::glfwMakeContextCurrent(_windowHandle);

        ::resolve_global_gl_functions();
        ::setup_debug();

        // TODO skybox normals are facing outwards !!!
        //      so enabling backface culling will cull the skybox !
        // ::glEnable(GL_CULL_FACE);
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

    auto Window::native_handle() const -> HandleType
    {
        return _windowHandle;
    }
    auto Window::width() const -> std::uint32_t
    {
        return _width;
    }
    auto Window::height() const -> std::uint32_t
    {
        return _height;
    }

    auto Window::show_cursor(bool show) const -> void
    {
        ::glfwSetInputMode(_windowHandle, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
}
