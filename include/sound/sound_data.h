#pragma once

#include <cstddef>
#include <span>

namespace game
{
    struct SoundData
    {
        std::span<const std::byte> format;
        std::span<const std::byte> data;
    };
}