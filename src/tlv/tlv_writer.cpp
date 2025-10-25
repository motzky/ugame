#include "tlv/tlv_writer.h"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "tlv/tlv_entry.h"

namespace
{
    auto write_bytes(std::vector<std::byte> &buffer, std::span<const std::byte> data) -> void
    {
        buffer.insert(std::ranges::end(buffer), std::ranges::cbegin(data), std::ranges::cend(data));
    }

    auto write_entry(std::vector<std::byte> &buffer, game::TlvType type, std::uint32_t length, std::span<const std::byte> value) -> void
    {
        write_bytes(buffer, {reinterpret_cast<const std::byte *>(&type), sizeof(type)});
        write_bytes(buffer, {reinterpret_cast<const std::byte *>(&length), sizeof(length)});
        write_bytes(buffer, value);
    }

}

namespace game
{
    TlvWriter::TlvWriter()
        : _buffer{}
    {
    }

    auto TlvWriter::yield() -> std::vector<std::byte>
    {
        auto tmp = std::vector<std::byte>{};
        std::ranges::swap(tmp, _buffer);
        return tmp;
    }

    auto TlvWriter::write(std::uint32_t value) -> void
    {
        const auto type = TlvType::UINT32;
        const auto length = static_cast<std::uint32_t>(sizeof(value));
        const auto value_bytes = std::span<const std::byte>{reinterpret_cast<const std::byte *>(&value), length};
        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(std::span<const std::uint32_t> value) -> void
    {
        const auto type = TlvType::UINT32_ARRAY;
        const auto length = static_cast<std::uint32_t>(value.size_bytes());
        const auto value_bytes = std::span<const std::byte>{reinterpret_cast<const std::byte *>(value.data()), length};
        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(std::string_view value) -> void
    {
        const auto type = TlvType::STRING;
        const auto length = static_cast<std::uint32_t>(value.length());
        const auto value_bytes = std::span<const std::byte>{reinterpret_cast<const std::byte *>(value.data()), length};
        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(std::span<const std::byte> value) -> void
    {
        const auto type = TlvType::BYTE_ARRAY;
        const auto length = static_cast<std::uint32_t>(value.size());
        const auto value_bytes = std::span<const std::byte>{reinterpret_cast<const std::byte *>(value.data()), length};
        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(TextureFormat value) -> void
    {
        const auto type = TlvType::TEXTURE_FORMAT;
        const auto length = static_cast<std::uint32_t>(sizeof(value));
        const auto value_bytes = std::span<const std::byte>{reinterpret_cast<const std::byte *>(&value), length};
        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(TextureUsage value) -> void
    {
        const auto type = TlvType::TEXTURE_USAGE;
        const auto length = static_cast<std::uint32_t>(sizeof(value));
        const auto value_bytes = std::span<const std::byte>{reinterpret_cast<std::byte *>(&value), length};
        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(TextureDescription &data) -> void
    {
        auto sub_writer = TlvWriter{};
        sub_writer.write(data.name);
        sub_writer.write(data.width);
        sub_writer.write(data.height);
        sub_writer.write(data.format);
        sub_writer.write(data.usage);
        sub_writer.write(data.data);

        const auto type = TlvType::TEXTURE_DESCRIPTION;
        const auto value_bytes = sub_writer.yield();
        const auto length = static_cast<std::uint32_t>(value_bytes.size());

        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(const VertexData &value) -> void
    {
        const auto type = TlvType::VERTEX_DATA;
        const auto length = static_cast<std::uint32_t>(sizeof(value));
        const auto value_bytes = std::span<const std::byte>{reinterpret_cast<const std::byte *>(&value), length};
        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(std::span<const VertexData> value) -> void
    {
        const auto type = TlvType::VERTEX_DATA_ARRAY;
        const auto length = static_cast<std::uint32_t>(value.size_bytes());
        const auto value_bytes = std::span<const std::byte>{reinterpret_cast<const std::byte *>(value.data()), length};
        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(std::string_view name, const MeshData &value) -> void
    {
        auto sub_writer = TlvWriter{};
        sub_writer.write(name);
        sub_writer.write(value.vertices);
        sub_writer.write(value.indices);

        const auto type = TlvType::MESH_DATA;
        const auto value_bytes = sub_writer.yield();
        const auto length = static_cast<std::uint32_t>(value_bytes.size());

        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(std::string_view name, std::string_view value) -> void
    {
        auto sub_writer = TlvWriter{};
        sub_writer.write(name);
        sub_writer.write(value);

        const auto type = TlvType::TEXT_FILE;
        const auto value_bytes = sub_writer.yield();
        const auto length = static_cast<std::uint32_t>(value_bytes.size());

        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(std::string_view name, std::span<const std::string> sub_mesh_names) -> void
    {
        auto sub_writer = TlvWriter{};
        sub_writer.write(name);
        sub_writer.write(static_cast<std::uint32_t>(sub_mesh_names.size()));
        for (const auto &str : sub_mesh_names)
        {
            sub_writer.write(str);
        }

        const auto type = TlvType::OBJECT_SUB_MESH_NAMES;
        const auto value_bytes = sub_writer.yield();
        const auto length = static_cast<std::uint32_t>(value_bytes.size());

        write_entry(_buffer, type, length, value_bytes);
    }

    auto TlvWriter::write(std::string_view name, const SoundData &data) -> void
    {
        auto sub_writer = TlvWriter{};
        sub_writer.write(name);
        sub_writer.write(data.format);
        sub_writer.write(data.data);

        const auto type = TlvType::SOUND_DATA;
        const auto value_bytes = sub_writer.yield();
        const auto length = static_cast<std::uint32_t>(value_bytes.size());

        write_entry(_buffer, type, length, value_bytes);
    }
}
