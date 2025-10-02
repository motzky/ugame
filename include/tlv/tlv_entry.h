#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "graphics/mesh_data.h"
#include "graphics/texture.h"

namespace game
{
    struct TextFile
    {
        std::string name;
        std::string data;
    };

    enum class TlvType : std::uint32_t
    {
        UINT32,
        UINT32_ARRAY,
        STRING,
        BYTE_ARRAY,
        TEXTURE_FORMAT,
        TEXTURE_USAGE,
        VERTEX_DATA,
        VERTEX_DATA_ARRAY,

        TEXTURE_DESCRIPTION,
        MESH_DATA,
        TEXT_FILE,
    };

    class TlvEntry
    {
    public:
        TlvEntry(TlvType type, std::span<const std::byte> value);

        auto type() const -> TlvType;
        auto length() const -> std::uint32_t;

        auto uint32_value() const -> std::uint32_t;
        auto uint32_array_value() const -> std::vector<std::uint32_t>;
        auto string_value() const -> std::string;
        auto byte_array_value() const -> std::vector<std::byte>;
        auto texture_usage_value() const -> TextureUsage;
        auto texture_format_value() const -> TextureFormat;
        auto texture_description_value() const -> TextureDescription;
        auto is_texture(std::string_view name) const -> bool;
        auto vertex_data_value() const -> VertexData;
        auto vertex_data_array_value() const -> std::vector<VertexData>;
        auto mesh_value() const -> MeshData;
        auto is_mesh(std::string_view name) const -> bool;
        auto text_file_value() const -> TextFile;
        auto is_text_file(std::string_view name) const -> bool;

        auto size() const -> std::uint32_t;

        auto to_string() -> std::string;

    private:
        TlvType _type;
        std::span<const std::byte> _value;
    };

}
