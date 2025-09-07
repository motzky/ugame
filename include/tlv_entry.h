#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace game
{
    enum class TlvType : std::uint32_t
    {
        UINT32,
        STRING
    };

    class TlvEntry
    {
    public:
        TlvEntry(TlvType type, std::span<const std::byte> value);

        auto type() const -> TlvType;

        auto uint32_value() const -> std::uint32_t;
        auto string_value() const -> std::string;

        auto size() const -> std::uint32_t;

    private:
        TlvType _type;
        std::span<const std::byte> _value;
    };

}
