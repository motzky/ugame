#ifdef WIN32
#error This code unit is NOT for Windows
#endif

#include "debug_ui.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "ImGuizmo.h"

#include "camera.h"
#include "matrix4.h"
#include "scene.h"
#include "window.h"

namespace game
{
    DebugUi::DebugUi(Window::HandleType window, Scene &scene, Camera &camera)
        : _scene(scene), _camera(camera)
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

        ::ImGuizmo::BeginFrame();
        ::ImGuizmo::SetOrthographic(false);
        ::ImGuizmo::Enable(true);
        ::ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        ::ImGui::LabelText("FPS", "%0.1f", io.Framerate);
        for (auto &point : _scene.points)
        {
            auto translate = Matrix4{point.position};

            ::ImGuizmo::Manipulate(
                _camera.view().data(),
                _camera.projection().data(),
                ::ImGuizmo::TRANSLATE,
                ::ImGuizmo::WORLD,
                const_cast<float *>(translate.data().data()),
                nullptr, nullptr, nullptr, nullptr);

            point.position.x = translate.data()[12];
            point.position.y = translate.data()[13];
            point.position.z = translate.data()[14];

            static float color[3]{};
            if (::ImGui::ColorPicker3("", color))
            {
                point.color.r = color[0];
                point.color.g = color[1];
                point.color.b = color[2];
            }

            ::ImGui::SliderFloat("const", &point.const_attenuation, 0.f, 1.f);
            ::ImGui::SliderFloat("linear", &point.linear_attenuation, 0.f, 1.f);
            ::ImGui::SliderFloat("quad", &point.quad_attenuation, 0.f, 1.f);
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