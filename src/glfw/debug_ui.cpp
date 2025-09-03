#ifdef WIN32
#error This code unit is NOT for Windows
#endif

#include "debug_ui.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include "backends/imgui_impl_glfw.h"

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

        ::ImGui_ImplGlfw_InitForOpenGL(window, true);
        ::ImGui_ImplOpenGL3_Init();
    }

    DebugUi ::~DebugUi()
    {
        ::ImGui_ImplOpenGL3_Shutdown();
        ::ImGui_ImplGlfw_Shutdown();
        ::ImGui::DestroyContext();
    }

    auto DebugUi::render() const -> void
    {
        ::ImGui_ImplOpenGL3_NewFrame();
        ::ImGui_ImplGlfw_NewFrame();
        ::ImGui::NewFrame();

        bool show_demo = true;
        ::ImGui::ShowDemoWindow(&show_demo);

        ::ImGui::Render();

        ::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
    }
}