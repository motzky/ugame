#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace game
{
    enum class TlvType : std::uint32_t
    {
        UINT32,
        STRING,
        BYTE_ARRAY,

    };

    class TlvEntry
    {
    public:
        TlvEntry(TlvType type, std::span<const std::byte> value);

        auto type() const -> TlvType;

        auto uint32_value() const -> std::uint32_t;
        auto string_value() const -> std::string;
        auto byte_array_value() const -> std::vector<std::byte>;

        auto size() const -> std::uint32_t;

    private:
        TlvType _type;
        std::span<const std::byte> _value;
    };

}
