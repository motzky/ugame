#include "buffer.h"

#include <cstddef>
#include <cstdint>
#include <span>

#include "auto_release.h"
#include "ensure.h"
#include "opengl.h"

namespace game
{
    Buffer::Buffer(std::uint32_t size)
        : _buffer{0u, [](auto vbo)
                  { ::glDeleteBuffers(1, &vbo); }},
          _size(size)
    {
        ::glCreateBuffers(1, &_buffer);
        ::glNamedBufferStorage(_buffer, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
    auto Buffer::write(std::span<const std::byte> data, std::size_t offset) const -> void
    {
        ensure(_size >= data.size_bytes() + offset, "buffer to small");
        ::glNamedBufferSubData(_buffer, offset, data.size(), data.data());
    }

    auto Buffer::native_handle() const -> ::GLuint
    {
        return _buffer;
    }

}