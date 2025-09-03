#ifndef WIN32
#error This code unit is ONLY for Windows !
#endif

#include "debug_ui.h"

#include <windows.h>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include "backends/imgui_impl_win32.h"

#include "scene.h"
#include "window.h"

namespace game
{
    DebugUi::DebugUi(Window::HandleType window, Scene &scene)
        : _scene(scene)
    {
        IMGUI_CHECKVERSION();
        ::ImGui::CreateContext();

        auto io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ::ImGui::StyleColorsDark();

        ::ImGui_ImplWin32_InitForOpenGL(window);
        ::ImGui_ImplOpenGL3_Init();
    }

    DebugUi ::~DebugUi()
    {
        ::ImGui_ImplOpenGL3_Shutdown();
        ::ImGui_ImplWin32_Shutdown();
        ::ImGui::DestroyContext();
    }

    auto DebugUi::render() const -> void
    {
        ::ImGui_ImplOpenGL3_NewFrame();
        ::ImGui_ImplWin32_NewFrame();
        ::ImGui::NewFrame();

        // bool show_demo = true;
        // ::ImGui::ShowDemoWindow(&show_demo);
        static float color[3]{};
        if (::ImGui::ColorPicker3("", color))
        {
            _scene.point.color.r = color[0];
            _scene.point.color.g = color[1];
            _scene.point.color.b = color[2];
        }
        ::ImGui::Render();

        ::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
    }

    auto DebugUi::add_mouse_event(const MouseButtonEvent &event) const -> void
    {
        auto io = ImGui::GetIO();

        io.AddMouseButtonEvent(0, event.state() == game::MouseButtonState::DOWN);
    }
}