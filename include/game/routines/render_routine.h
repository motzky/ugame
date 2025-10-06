#pragma once

#include "graphics/debug_ui.h"
#include "graphics/renderer.h"
#include "graphics/shape_wireframe_renderer.h"
#include "loaders/mesh_loader.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "tlv/tlv_reader.h"
#include "window.h"

namespace game::routines
{
    class LevelRoutine;

    class RenderRoutine : public messaging::Subscriber
    {
    public:
        RenderRoutine(const LevelRoutine &level_routine, const Window &window, messaging::MessageBus &bus, Scheduler &scheduler, const TlvReader &reader, MeshLoader &mesh_loader);
        RenderRoutine(const RenderRoutine &) = delete;
        auto operator=(const RenderRoutine &) -> RenderRoutine & = delete;
        RenderRoutine(RenderRoutine &&) = default;

        auto create_task() -> Task;

        virtual auto handle_key_press(const KeyEvent &) -> void override;
        virtual auto handle_mouse_move(const MouseEvent &) -> void override;
        virtual auto handle_quit() -> void override;

    private:
        const LevelRoutine &_level_routine;
        const Window &_window;
        messaging::MessageBus &_bus;
        Scheduler &_scheduler;
        Renderer _renderer;
        ShapeWireframeRenderer _debug_wireframe_renderer;
        // DebugUi _debug_ui;
        bool _show_physics_debug;
        bool _show_debug;
        bool _running;
        messaging::AutoSubscribe _auto_subscribe;
    };
}
