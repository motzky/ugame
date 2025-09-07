#include "tlv/tlv_entry.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <string_view>

#include "ensure.h"
#include "texture.h"
#include "tlv/tlv_reader.h"

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

    auto TlvEntry::length() const -> std::uint32_t
    {
        return static_cast<std::uint32_t>(_value.size_bytes());
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

    auto TlvEntry::texture_format_value() const -> TextureFormat
    {
        ensure(_type == TlvType::TEXTURE_FORMAT, "incorrect type");
        ensure(_value.size() == sizeof(TextureFormat), "incorrect size");

        auto value = TextureFormat{};
        std::memcpy(&value, _value.data(), sizeof(value));

        return value;
    }

    auto TlvEntry::texture_usage_value() const -> TextureUsage
    {
        ensure(_type == TlvType::TEXTURE_USAGE, "incorrect type");
        ensure(_value.size() == sizeof(TextureUsage), "incorrect size");

        auto value = TextureUsage{};
        std::memcpy(&value, _value.data(), sizeof(value));

        return value;
    }

    auto TlvEntry::texture_data_value() const -> TextureData
    {
        ensure(_type == TlvType::TEXTURE_DATA, "incorrect type");

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "texture TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto name = (*reader_cursor).string_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "texture TLV too small");
        const auto width = (*reader_cursor).uint32_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "texture TLV too small");
        const auto height = (*reader_cursor).uint32_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "texture TLV too small");
        const auto format = (*reader_cursor).texture_format_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "texture TLV too small");
        const auto usage = (*reader_cursor).texture_usage_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "texture TLV too small");
        const auto data = (*reader_cursor).byte_array_value();

        ++reader_cursor;
        ensure(reader_cursor == std::ranges::end(reader), "texture TLV too large");

        return {name, format, usage, width, height, data};
    }

    auto TlvEntry::is_texture(std::string_view name) -> bool
    {
        ensure(_type == TlvType::TEXTURE_DATA, "incorrect type");

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "texture TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto texture_name = (*reader_cursor).string_value();
        return name == texture_name;
    }

    auto TlvEntry::size() const -> std::uint32_t
    {
        return static_cast<std::uint32_t>(sizeof(_type)) + static_cast<std::uint32_t>(sizeof(std::uint32_t)) + _value.size();
    }

}
