#include "buffer.h"

#include <cstdint>
#include <span>
#include <cstddef>

#include "auto_release.h"
#include "opengl.h"

namespace game
{
    Buffer::Buffer(std::uint32_t size)
    {
    }
    auto Buffer::write(std::span<const std::byte>, std::size_t offset) const -> void
    {
    }

}