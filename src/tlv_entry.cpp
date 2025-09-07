#include "tlv_entry.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>

#include "ensure.h"

namespace game
{
    TlvEntry::TlvEntry(TlvType type, std::span<const std::byte> value)
        : _type{type},
          _value(value)
    {
    }

    auto TlvEntry::type() const -> TlvType
    {
        return _type;
    }

    auto TlvEntry::size() const -> std::uint32_t
    {
        return static_cast<std::uint32_t>(sizeof(_type)) + static_cast<std::uint32_t>(sizeof(std::uint32_t)) + _value.size();
    }

    auto TlvEntry::uint32_value() const -> std::uint32_t
    {
        ensure(_type == TlvType::UINT32, "incorrect type");
        ensure(_value.size() == sizeof(std::uint32_t), "incorrect size");

        auto value = std::uint32_t{};
        std::memcpy(&value, _value.data(), sizeof(value));

        return value;
    }

    auto TlvEntry::string_value() const -> std::string
    {
        ensure(_type == TlvType::STRING, "incorrect type");

        const auto *ptr = reinterpret_cast<const char *>(_value.data());
        return std::string(ptr, ptr + _value.size());
    }

    auto TlvEntry::byte_array_value() const -> std::vector<std::byte>
    {
        ensure(_type == TlvType::BYTE_ARRAY, "incorrect type");
        const auto *ptr = reinterpret_cast<const std::byte *>(_value.data());
        return std::vector<std::byte>(ptr, ptr + _value.size());
    }
}
