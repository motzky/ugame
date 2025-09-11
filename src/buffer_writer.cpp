#include "buffer_writer.h"

#include <cstddef>
#include <cstdint>
#include <span>

#include "auto_release.h"
#include "graphics/buffer.h"
#include "graphics/opengl.h"

namespace game
{
    BufferWriter::BufferWriter(const Buffer &buffer)
        : _buffer(buffer), _offset{}
    {
    }

}