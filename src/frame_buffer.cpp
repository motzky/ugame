#include "frame_buffer.h"

#include <cstdint>

#include "opengl.h"
#include "auto_release.h"

namespace game
{
    FrameBuffer::FrameBuffer(std::uint32_t width, std::uint32_t height)
        : _handle{0u, [](const auto buffer)
                  { ::glDeleteFrameBuffers(1, &buffer); }}
    {
    }

    auto FrameBuffer::native_handle() -> ::GLuint
    {
        return _handle;
    }

    auto FrameBuffer::bind() -> void
    {
    }

    auto FrameBuffer::unbind() -> void
    {
    }

}