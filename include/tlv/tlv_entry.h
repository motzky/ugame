#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "texture.h"

namespace game
{
    enum class TlvType : std::uint32_t
    {
        UINT32,
        STRING,
        BYTE_ARRAY,
        TEXTURE_FORMAT,
        TEXTURE_USAGE,

        TEXTURE_DATA
    };

    class TlvEntry
    {
    public:
        TlvEntry(TlvType type, std::span<const std::byte> value);

        auto type() const -> TlvType;
        auto length() const -> std::uint32_t;

        auto uint32_value() const -> std::uint32_t;
        auto string_value() const -> std::string;
        auto byte_array_value() const -> std::vector<std::byte>;
        auto texture_usage_value() const -> TextureUsage;
        auto texture_format_value() const -> TextureFormat;
        auto texture_data_value() const -> TextureData;
        auto is_texture(std::string_view name) -> bool;

        auto size() const -> std::uint32_t;

    private:
        TlvType _type;
        std::span<const std::byte> _value;
    };

}
