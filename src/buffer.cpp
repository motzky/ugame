#include "buffer.h"

#include <cstdint>
#include <span>
#include <cstddef>

#include "auto_release.h"
#include "opengl.h"

namespace game
{
    Buffer::Buffer(std::uint32_t size)
        : _buffer{0u, [](auto vbo)
                  { ::glDeleteBuffers(1, &vbo); }}
    {
        ::glCreateBuffers(1, &_buffer);
        ::glNamedBufferStorage(_buffer, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
    auto Buffer::write(std::span<const std::byte> data, std::size_t offset) const -> void
    {
        ::glNamedBufferSubData(_buffer, offset, data.size(), data.data());
    }

    auto Buffer::native_handle() const -> ::GLuint
    {
        return _buffer;
    }

}