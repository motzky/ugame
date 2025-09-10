#pragma once

#include "pass_key.h"

namespace game
{
    class PhysicsSystem;

    class RigidBody
    {
    public:
        RigidBody(PassKey<PhysicsSystem>);

    private:
    };
}