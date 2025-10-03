#include "utils/decompress.h"

#include <cstdint>
#include <span>
#include <vector>

#include <zstd.h>

#include "utils/ensure.h"
#include "utils/exception.h"

namespace game
{
    auto decompress([[maybe_unused]] std::span<const std::byte> data) -> std::vector<std::byte>
    {
        const auto decompressed_buffer_size = ::ZSTD_getFrameContentSize(data.data(), data.size_bytes());

        expect(decompressed_buffer_size != ZSTD_CONTENTSIZE_ERROR, "not compressed by zstd");
        expect(decompressed_buffer_size != ZSTD_CONTENTSIZE_UNKNOWN, "cannot get original size");

        auto decompressed_buffer = std::vector<std::byte>(decompressed_buffer_size);

        [[maybe_unused]] const auto decompressed_result = ::ZSTD_decompress(
            decompressed_buffer.data(),
            decompressed_buffer.size(),
            data.data(),
            decompressed_buffer_size);

        return decompressed_buffer;
    }
}