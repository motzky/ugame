#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace game
{
    auto decompress(std::span<const std::byte> data) -> std::vector<std::byte>;
}