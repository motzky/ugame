#include "utils/compress.h"

#include <cstdint>
#include <span>
#include <vector>

#include <zstd.h>

#include "utils/exception.h"

namespace game
{
    auto compress(std::span<const std::byte> data) -> std::vector<std::byte>
    {
        const auto compressed_buffer_size = ::ZSTD_compressBound(data.size_bytes());
        auto compressed_buffer = std::vector<std::byte>(compressed_buffer_size);

        const auto compressed_result = ::ZSTD_compress(
            compressed_buffer.data(),
            compressed_buffer.size(),
            data.data(),
            data.size_bytes(),
            ::ZSTD_defaultCLevel());

        if (::ZSTD_isError(compressed_result) != 0)
        {
            throw Exception("failed to compress data: {}", ::ZSTD_getErrorName(compressed_result));
        }

        compressed_buffer.resize(compressed_result);

        return compressed_buffer;
    }
}