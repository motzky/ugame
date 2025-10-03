#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace game
{
    auto compress(std::span<const std::byte> data) -> std::vector<std::byte>;
}