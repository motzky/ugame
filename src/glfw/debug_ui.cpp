#ifdef WIN32
#error This code unit is NOT for Windows
#endif

#include "debug_ui.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include "backends/imgui_impl_glfw.h"

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

        ::ImGui_ImplGlfw_InitForOpenGL(window, false);
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
        auto io = ImGui::GetIO();

        ::ImGui_ImplOpenGL3_NewFrame();
        ::ImGui_ImplGlfw_NewFrame();
        ::ImGui::NewFrame();

        ::ImGui::LabelText("FPS", "%0.1f", io.Framerate);

        // bool show_demo = true;
        // ::ImGui::ShowDemoWindow(&show_demo);
        static float color[3]{};
        if (::ImGui::ColorPicker3("", color))
        {
            _scene.point.color.r = color[0];
            _scene.point.color.g = color[1];
            _scene.point.color.b = color[2];
        }

        ::ImGui::SliderFloat("const", &_scene.point.const_attenuation, 0.f, 1.f);
        ::ImGui::SliderFloat("linear", &_scene.point.linear_attenuation, 0.f, 1.f);
        ::ImGui::SliderFloat("quad", &_scene.point.quad_attenuation, 0.f, 1.f);

        ::ImGui::Render();

        ::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
    }

    auto DebugUi::add_mouse_event(const MouseButtonEvent &event) const -> void
    {
        auto io = ImGui::GetIO();

        io.AddMouseButtonEvent(0, event.state() == game::MouseButtonState::DOWN);
    }
}