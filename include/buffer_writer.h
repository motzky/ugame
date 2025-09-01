#pragma once

#include <cstddef>
#include <cstdint>
#include <ranges>
#include <span>

#include "auto_release.h"
#include "buffer.h"
#include "opengl.h"

namespace game
{
    class BufferWriter
    {
    public:
        BufferWriter(const Buffer &buffer);

        template <class T, std::size_t N>
        auto write(const T (&data)[N]) -> void
        {
            _buffer.write({reinterpret_cast<const std::byte *>(data), sizeof(T) * N}, _offset);
            _offset += sizeof(T) * N;
        }

    private:
        const Buffer &_buffer;
        std::uint32_t _offset;
    };
}