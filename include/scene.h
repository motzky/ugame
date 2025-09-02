#pragma once

#include <vector>

#include "color.h"
#include "entity.h"

namespace game
{
    struct Scene
    {
        std::vector<const Entity *> entities;
        Color ambient;
    };
}