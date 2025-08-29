#pragma once

#include <vector>

#include "entity.h"

namespace game
{
    struct Scene
    {
        std::vector<const Entity *> entities;
    };
}