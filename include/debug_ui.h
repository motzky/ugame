#pragma once

#include "imgui.h"

#include "camera.h"
#include "events/mouse_button_event.h"
#include "graphics/scene.h"
#include "window.h"

namespace game
{
    class DebugUi
    {
    public:
        DebugUi(Window::HandleType window, Scene &_scene, Camera &camera, float &gamma);
        ~DebugUi();

        auto render() const -> void;
        auto add_mouse_event(const MouseButtonEvent &event) const -> void;

    private:
        Scene &_scene;
        Camera &_camera;
        float &_gamma;
    };
}