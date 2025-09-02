#include "buffer_writer.h"

#include <cstdint>
#include <span>
#include <cstddef>

#include "auto_release.h"
#include "opengl.h"

namespace game
{
    BufferWriter::BufferWriter(const Buffer &buffer)
        : _buffer(buffer), _offset{}
    {
    }

}