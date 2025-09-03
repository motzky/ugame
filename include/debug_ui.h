#pragma once

#include "imgui.h"

#include "camera.h"
#include "mouse_button_event.h"
#include "scene.h"
#include "window.h"

namespace game
{
    class DebugUi
    {
    public:
        DebugUi(Window::HandleType window, Scene &_scene, Camera &camera);
        ~DebugUi();

        auto render() const -> void;
        auto add_mouse_event(const MouseButtonEvent &event) const -> void;

    private:
        Scene &_scene;
        Camera &_camera;
    };
}