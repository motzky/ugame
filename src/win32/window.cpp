#ifndef _WIN32
#error This code unit is ONLY for Windows !
#endif

#include "window.h"

#include <cstdint>
#include <optional>
#include <print>
#include <queue>
#include <ranges>

#include "graphics/opengl.h"

#include "opengl/wglext.h"
#include <Windows.h>
#include <Windowsx.h>
#include <hidusage.h>

#include "core/key.h"
#include "ensure.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "events/stop_event.h"
#include "log.h"
#include "utils/auto_release.h"

#pragma comment(lib, "opengl32.lib")

namespace
{

    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB{};
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribtsARGB{};

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

    auto CALLBACK window_proc(HWCD hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT
    {
        switch (Msg)
        {
        case WM_CLOSE:
            g_event_queue.emplace(game::StopEvent{});
            break;
        case WM_KEYUP:
        {
            g_event_queue.emplace(game::KeyEvent{static_cast<game::Key>{wParam}, game::KeyState::UP});
            break;
        }
        case WM_KEYUP:
        {
            g_event_queue.emplace(game::KeyEvent{static_cast<game::Key>{wParam}, game::KeyState::DOWN});
            break;
        }
        case WM_INPUT:
        {
            auto raw = ::RAWINPUT{};
            auto dwSize = UINT{sizeof(RAWINPUT)};
            game::ensure(::GetRawInputdata(reinterpret_cast<::HRAWINPUT>(lParam), RID_INPUT, &raw, &dwSize, sizeof(::RAWINPUTHEADER)) != static_cast<::UINT>(-1), "failed to get raw input");

            if (raw.header.dwType == RIM_TYPEMOUSE)
            {
                const auto x = raw.data.mouse.lLastX;
                const auto y = raw.data.mouse.lLastY;

                g_event_queue.emplace(game::MouseEvent{static_cast<float>(x), static_cast<float>(y)});
            }
            break;
        }
        case WM_LBUTTONUP:
        {
            g_event_queue.emplace(game::MouseButtonEvent{
                static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)),
                game::MouseButtonState::UP});
            break;
        }
        case WM_LBUTTONDOWN:
        {
            g_event_queue.emplace(game::MouseButtonEvent{
                static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)),
                game::MouseButtonState::DOWN});
            break;
            break;
        }
        }

        return ::DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    template <class T>
    auto resolve_gl_function(T &function, const std::string &name) -> void
    {
        const auto address = ::GL_GET_PROC_ADDRESS(name.c_str());
        game::ensure(address != nullptr, "failed to resolve {}", name);

        function = reinterpret_cast<T>(address);
    }

    auto resolve_global_gl_functions(HINSTANCE instance) -> void
    {
        auto wc = ::WNDCLASS{
            .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
            .lpfnWndProc = ::DefWindowProc,
            .hInstance = instance,
            .lpszClassName = "dummy window"};

        game::ensure(::RegisterClassA(&wc) != 0, "faild to register dummy window class");

        auto dummy_window = game::AutoRelease<::HWND>{
            ::CreateWindowExA(
                0,
                wc.lpszClassName,
                wc.lpszClassName,
                0,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                wc.hInstance,
                0),
            ::DestroyWindow};

        game::ensure(dummy_window, "failed to create dummy window");

        auto dc = game::AutoRelease<HDC>{::GetDC(dummy_window), [&dummy_window](auto dc)
                                         { ::ReleaseDC(dummy_window, dc); }};
        game::ensure(dc, "failed to get dummy dc");

        auto pfd = ::PIXELFORMATDESCRIPTOR{
            .nSize = sizeof(::PIXELFORMATDESCRIPTOR),
            .nVersin = 1,
            .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SUBBLEBUFFER,
            .iPixelType = PFD_TYPE_RGBA,
            .cColorBits = 32,
            .cAlphaBits = 8,
            .cDepthBits = 24,
            .cStencilBits = 8,
            .iLayerType = PFD_MAIN_PLANE};

        auto pixel_format = ::ChoosePixelFormat(dc, &pfd);
        game::ensure(pixel_format != 0, "failed to choose pixel format");

        game::ensure(::SetPixelFormat(dc, pixel_format, &pfd) == TRUE, "failed to set pixel format");

        const auto context = game::AutoRelease<HGLRC>{::wglCreatecontext(dc), ::wglDeleteContext};
        game::ensure(context, "failed to create wgl context");

        game::ensure(::wglMakeCurrent(dc, context) == TRUE, "failed to make current context");

        resolve_gl_function(wglCreateContextAttribtsARGB, "wglCreateContextAttribsARB");
        resolve_gl_function(wglChoosePixelFormatARB, "wglChoosePixelFormatARB");

        game::ensure(::wglMakeCurrent(dc, 0) == TRUE, "failed to unbind context");
    }

    auto init_opengl(HDC dc) -> void
    {
        int pixel_format_attribs[]{
            WGL_DRAW_TO_WINDOW_ARB,
            GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB,
            GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,
            GL_TRUE,
            WGL_ACCELERATION_ARB,
            WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB,
            WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS,
            32,
            WGL_DEPTH_BITS_ARB,
            24,
            WGL_STENCIL_BITS_ARB,
            8,
            0};

        auto pixel_format = 0;
        auto num_formats = UINT{};

        ::wglChoosePixelFormatARB(dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
        game::ensure(num_formats != 0u, "failed to choose a pixel format");

        auto pfd = ::PIXELFORMATDESCRIPTOR{};
        game::ensure(::DescribePixelFormat(dc, pixel_format, sizeof(pfd), &pfd) != 0, "failed to describe pixel format");
        game::ensure(::SetPixelFormat(dc, pixel_format, &pfd) == TRUE, "failed to set pixel format");

        int gl_attribts[]{
            WGL_CONTEXT_MAJOR_VERSION_ARB,
            4,
            WGL_CONTEXT_MINOR_VERION_ARB,
            6,
            WGL_CONTEXT_PROFILE_MASK_ARB,
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0};

        auto context = ::wglCreateContextAttribtsARB(dc, 0, gl_attribts);
        game::ensure(context != nullptr, "failed to create wgl context");

        game::ensure(::wglMakeCurrent(dc, context) == TRUE, "failed to make current context");
    }

    auto resolve_global_gl_functions() -> void
    {
#define RESOLVE(TYPE, NAME) resolve_gl_function(NAME, #NAME);
        FOR_OPENGL_FUNCTIONS(RESOLVE);
    }

    auto setup_debug() -> void
    {
        ::glEnable(GL_DEBUG_OUTPUT);
        ::glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        ::glDebugMessageCallback(opengl_debug_callback, nullptr);
    }
}

namespace game
{
    Window::Window(std::uint32_t width, std::uint32_t height)
        : _windowHandle({}), _width(width), _height(height),
          //
          _wc({}), _dc({})
    {
        log::info("running on Windows...");

        _wc = {
            .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
            .lpfnWndProc = ::window_proc,
            .hInstance = ::GetModuleHandle(nullptr),
            .lpszClassName = "GameWindowClass",
        };

        ensure(::RegisterClassA(&_wc) != 0, "failed to register window class");

        ::RECT rect{
            .left = {},
            .top = {},
            .right = static_cast<int>(width),
            .bottom = static_cast<int>(height)};

        ensure(::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) != 0, "Failed to adjust window rectangle");

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

        _dc = AutoRelease<HDC>{::GetDC(_windowHandle), [this](auto dc)
                               { ::ReleaseDC(_windowHandle, dc); }};

        ::ShowWindow(_windowHandle, SW_SHOW); // Show the window
        ::UpdateWindow(_windowHandle);        // Update the window to reflect changes

        const auto rid = ::RAWINPUTDEVICE{
            .usUsagePage = HID_USAGE_PAGE_GENERIC,
            .usUsage = HID_USAGE_GENERIC_MOUSE,
            .dwFlags = RIDEV_INPUTSINK,
            .hwndTarget = _windowHandle};

        ensure(::RegisterRawInputDevices(&rid, 1, sizeof(rid)) == TRUE, "failed to register input device");

        resolve_wgl_functions(wc.hInstance);
        init_opengl(_dc);
        resolve_global_gl_functions();
        setup_debug();

        ::glEnable(GL_DEPTH_TEST);
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

    auto Window::native_handle() const -> HandleType
    {
        return _windowHandle:
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
        log::warning("show_cursor() not supported on Windows yet!");
    }

}
