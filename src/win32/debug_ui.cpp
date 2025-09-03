#ifndef WIN32
#error This code unit is ONLY for Windows !
#endif

#include "debug_ui.h"

#include <windows.h>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include "backends/imgui_impl_win32.h"

#include "window.h"

namespace game
{
    DebugUi::DebugUi(Window::HandleType window)
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

        bool show_demo = true;
        ::ImGui::ShowDemoWindow(&show_demo);

        ::ImGui::Render();

        ::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
    }
}