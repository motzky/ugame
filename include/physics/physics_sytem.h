#pragma once

#include <memory>

#include "physics/debug_renderer.h"

namespace game
{
    class PhysicsSystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem();

        auto update() -> void;
        auto debug_renderer() const -> const DebugRenderer &;

    private:
        struct Implementation;
        std::unique_ptr<Implementation> _impl;
    };
}
