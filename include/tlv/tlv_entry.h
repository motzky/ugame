#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "mesh_data.h"
#include "texture.h"

namespace game
{
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
        MESH_DATA
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

        auto size() const -> std::uint32_t;

    private:
        TlvType _type;
        std::span<const std::byte> _value;
    };

}

template <>
struct std::formatter<game::TlvType>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::TlvType &obj, std::format_context &ctx) const
    {
        auto str = "unknown"sv;
        switch (obj)
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
        }
        return std::format_to(ctx.out(), "{}", str);
    }
};
