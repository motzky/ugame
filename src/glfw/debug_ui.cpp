#ifdef WIN32
#error This code unit is NOT for Windows
#endif

#include "debug_ui.h"

#include <cstring>
#include <format>
#include <ranges>
#include <string>

#include <ImGuizmo.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "camera.h"
#include "graphics/scene.h"
#include "math/vector3.h"
#include "window.h"

namespace game
{
    DebugUi::DebugUi(Window::HandleType window, Scene &scene, Camera &camera, float &gamma)
        : _scene(scene),
          _camera(camera),
          _gamma(gamma)
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

        static auto selected_point = std::size_t{};

        if (::ImGui::Button("Add Light"))
        {
            _scene.points.push_back(_scene.points.back());
            selected_point = _scene.points.size() - 1u;
        }

        ::ImGui::LabelText("FPS", "%0.1f", io.Framerate);

        ::ImGui::SliderFloat("gamma", &_gamma, 0.f, 5.f);

        if (::ImGui::CollapsingHeader("ambient"))
        {
            float color[3]{};
            std::memcpy(color, &_scene.ambient, sizeof(color));
            if (::ImGui::ColorPicker3("ambient color", color))
            {
                std::memcpy(&_scene.ambient, color, sizeof(color));
            }
        }

        if (::ImGui::CollapsingHeader("directional"))
        {
            float color[3]{};
            std::memcpy(color, &_scene.directional.color, sizeof(color));
            if (::ImGui::ColorPicker3("directional color", color))
            {
                std::memcpy(&_scene.directional.color, color, sizeof(color));
            }
        }

        for (const auto &[index, point] : _scene.points | std::views::enumerate)
        {
            float color[3]{};
            std::memcpy(color, &point.color, sizeof(color));

            const auto header_name = std::format("color {}", index);
            const auto picker_name = std::format("color {}", index);
            const auto const_name = std::format("constant {}", index);
            const auto linear_name = std::format("linear {}", index);
            const auto quad_name = std::format("quad {}", index);

            if (::ImGui::CollapsingHeader(header_name.c_str()))
            {
                if (::ImGui::ColorPicker3(picker_name.c_str(), color))
                {
                    std::memcpy(&point.color, color, sizeof(color));

                    selected_point = index;
                }

                ::ImGui::SliderFloat(const_name.c_str(), &point.const_attenuation, 0.f, 1.f);
                ::ImGui::SliderFloat(linear_name.c_str(), &point.linear_attenuation, 0.f, 1.f);
                ::ImGui::SliderFloat(quad_name.c_str(), &point.quad_attenuation, 0.f, .1f);
            }
        }

        auto &point = _scene.points[selected_point];
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

        ::ImGui::Render();
        ::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
    }

    auto DebugUi::add_mouse_event(const MouseButtonEvent &event) const -> void
    {
        auto io = ImGui::GetIO();

        io.AddMouseButtonEvent(0, event.state() == game::MouseButtonState::DOWN);
    }
}