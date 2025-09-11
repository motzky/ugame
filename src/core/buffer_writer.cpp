#include "core/buffer_writer.h"

#include <cstddef>
#include <cstdint>
#include <span>

#include "graphics/buffer.h"
#include "graphics/opengl.h"
#include "utils/auto_release.h"

namespace game
{
    BufferWriter::BufferWriter(const Buffer &buffer)
        : _buffer(buffer), _offset{}
    {
    }

}