#include "tlv/tlv_entry.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <span>
#include <string>
#include <string_view>

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "tlv/tlv_reader.h"
#include "utils/ensure.h"

using namespace std::literals;

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

    auto TlvEntry::uint32_array_value() const -> std::vector<std::uint32_t>
    {
        ensure(_type == TlvType::UINT32_ARRAY, "incorrect type");
        auto value = std::vector<std::uint32_t>(_value.size() / sizeof(std::uint32_t));
        std::memcpy(value.data(), _value.data(), _value.size_bytes());

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

    auto TlvEntry::texture_description_value() const -> TextureDescription
    {
        ensure(_type == TlvType::TEXTURE_DESCRIPTION, "incorrect type");

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

    auto TlvEntry::is_texture(std::string_view name) const -> bool
    {
        if (_type != TlvType::TEXTURE_DESCRIPTION)
        {
            return false;
        }

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "texture TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto texture_name = (*reader_cursor).string_value();
        return texture_name == name;
    }

    auto TlvEntry::vertex_data_value() const -> VertexData
    {
        ensure(_type == TlvType::VERTEX_DATA, "incorrect type");
        ensure(_value.size() == sizeof(VertexData), "incorrect size");

        auto value = VertexData{};
        std::memcpy(&value, _value.data(), sizeof(value));

        return value;
    }
    auto TlvEntry::vertex_data_array_value() const -> std::vector<VertexData>
    {
        ensure(_type == TlvType::VERTEX_DATA_ARRAY, "incorrect type");
        auto value = std::vector<VertexData>(_value.size() / sizeof(VertexData));
        std::memcpy(value.data(), _value.data(), _value.size_bytes());

        return value;
    }

    auto TlvEntry::mesh_value() const -> MeshData
    {
        ensure(_type == TlvType::MESH_DATA, "incorrect type");

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "mesh TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto name = (*reader_cursor).string_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "mesh TLV too small");

        ensure((*reader_cursor).type() == TlvType::VERTEX_DATA_ARRAY, "second member not vertex data array");
        const auto vertex_data = std::span<const VertexData>{
            reinterpret_cast<const VertexData *>((*reader_cursor)._value.data()),
            static_cast<std::uint32_t>((*reader_cursor)._value.size() / sizeof(VertexData))};
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "mesh TLV too small");

        ensure((*reader_cursor).type() == TlvType::UINT32_ARRAY, "third member not uint32 data array");
        const auto index_data = std::span<const std::uint32_t>{
            reinterpret_cast<const std::uint32_t *>((*reader_cursor)._value.data()),
            static_cast<std::uint32_t>((*reader_cursor)._value.size()) / sizeof(std::uint32_t)};
        ++reader_cursor;
        ensure(reader_cursor == std::ranges::end(reader), "mesh TLV too large");

        // log::debug("loaded mesh {} - {} verts, {} indices", name, vertex_data.size(), index_data.size());
        return {vertex_data, index_data};
    }

    auto TlvEntry::is_mesh(std::string_view name) const -> bool
    {
        if (_type != TlvType::MESH_DATA)
        {
            return false;
        }

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "mesh TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto mesh_name = (*reader_cursor).string_value();
        return mesh_name == name;
    }

    auto TlvEntry::text_file_value() const -> TextFile
    {
        ensure(_type == TlvType::TEXT_FILE, "incorrect type");

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "text_file TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto name = (*reader_cursor).string_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "text_file TLV too small");

        ensure((*reader_cursor).type() == TlvType::STRING, "second member not a string");
        const auto content = (*reader_cursor).string_value();
        ++reader_cursor;
        ensure(reader_cursor == std::ranges::end(reader), "text_file TLV too large");

        return {name, content};
    }

    auto TlvEntry::is_text_file(std::string_view name) const -> bool
    {
        if (_type != TlvType::TEXT_FILE)
        {
            return false;
        }

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "text file TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto text_file_name = (*reader_cursor).string_value();
        return text_file_name == name;
    }

    auto TlvEntry::is_object_data(std::string_view name) const -> bool
    {
        if (_type != TlvType::OBJECT_SUB_MESH_NAMES)
        {
            return false;
        }

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "object sub mesh TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto object_name = (*reader_cursor).string_value();
        return object_name == name;
    }

    auto TlvEntry::object_data_value() const -> std::vector<std::string>
    {
        ensure(_type == TlvType::OBJECT_SUB_MESH_NAMES, "incorrect type");

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "object sub mesh TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto name = (*reader_cursor).string_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "object sub mesh TLV too small");

        ensure((*reader_cursor).type() == TlvType::UINT32, "second member not uint32 data");
        const auto num_strings = (*reader_cursor).uint32_value();
        ++reader_cursor;

        if (num_strings > 0u)
        {
            ensure(reader_cursor != std::ranges::end(reader), "object sub mesh TLV too small");
        }

        auto value = std::vector<std::string>{};

        for (auto i = 0u; i < num_strings; ++i)
        {
            ensure((*reader_cursor).type() == TlvType::STRING, "string member {} not a string", i);
            const auto str = (*reader_cursor).string_value();
            value.push_back(str);
            ++reader_cursor;
            if (i < num_strings - 1)
            {
                ensure(reader_cursor != std::ranges::end(reader), "object sub mesh TLV too small");
            }
            else
            {
                ensure(reader_cursor == std::ranges::end(reader), "object sub mesh TLV too large");
            }
        }

        return value;
    }

    auto TlvEntry::sound_data_value() const -> SoundData
    {
        ensure(_type == TlvType::SOUND_DATA, "incorrect type");

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto name = (*reader_cursor).string_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "TLV too small");

        ensure((*reader_cursor).type() == TlvType::BYTE_ARRAY, "second member not byte data array");
        const auto format = (*reader_cursor).byte_array_value();
        ++reader_cursor;
        ensure(reader_cursor != std::ranges::end(reader), "TLV too small");

        ensure((*reader_cursor).type() == TlvType::BYTE_ARRAY, "third member not byte data array");
        const auto data = (*reader_cursor).byte_array_value();
        ++reader_cursor;
        ensure(reader_cursor == std::ranges::end(reader), "TLV too large");

        return {format, data};
    }

    auto TlvEntry::is_sound(std::string_view name) const -> bool
    {
        if (_type != TlvType::SOUND_DATA)
        {
            return false;
        }

        auto reader = TlvReader(_value);
        auto reader_cursor = std::ranges::begin(reader);
        ensure(reader_cursor != std::ranges::end(reader), "mesh TLV too small");
        ensure((*reader_cursor).type() == TlvType::STRING, "first member not a string");

        const auto mesh_name = (*reader_cursor).string_value();
        return mesh_name == name;
    }

    auto TlvEntry::size() const -> std::uint32_t
    {
        return static_cast<std::uint32_t>(sizeof(_type)) + static_cast<std::uint32_t>(sizeof(std::uint32_t)) + static_cast<std::uint32_t>(_value.size());
    }
    auto TlvEntry::to_string() -> std::string
    {
        auto str = "unknown"sv;
        switch (_type)
        {
            using enum game::TlvType;
        case UINT32:
            str = "UINT32"sv;
            break;
        case UINT32_ARRAY:
            str = "UINT32_ARRAY"sv;
            break;
        case STRING:
            str = "STRING"sv;
            break;
        case BYTE_ARRAY:
            str = "BYTE_ARRAY"sv;
            break;
        case TEXTURE_FORMAT:
            str = "TEXTURE_FORMAT"sv;
            break;
        case TEXTURE_USAGE:
            str = "TEXTURE_USAGE"sv;
            break;
        case TEXTURE_DESCRIPTION:
            str = "TEXTURE_DESCRIPTION"sv;
            break;
        case VERTEX_DATA:
            str = "VERTEX_DATA"sv;
            break;
        case VERTEX_DATA_ARRAY:
            str = "VERTEX_DATA_ARRAY"sv;
            break;
        case MESH_DATA:
            str = "MESH_DATA"sv;
            break;
        case TEXT_FILE:
            str = "TEXT_FILE"sv;
            break;
        case OBJECT_SUB_MESH_NAMES:
            str = "OBJECT_SUB_MESH_NAMES"sv;
            break;
        case SOUND_DATA:
            str = "SOUND_DATA"sv;
            break;
        }
        return std::format("{}", str);
    }

}
