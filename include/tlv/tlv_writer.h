#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "mesh_data.h"
#include "texture.h"
#include "tlv/tlv_entry.h"
#include "vertex_data.h"

namespace game
{
    class TlvWriter
    {
    public:
        TlvWriter();

        auto yield() -> std::vector<std::byte>;
        auto write(std::uint32_t value) -> void;
        auto write(std::span<const std::uint32_t> value) -> void;
        auto write(std::string_view value) -> void;
        auto write(std::span<const std::byte> value) -> void;
        auto write(TextureFormat value) -> void;
        auto write(TextureUsage value) -> void;
        auto write(TextureDescription &value) -> void;
        auto write(const VertexData &value) -> void;
        auto write(std::span<const VertexData> value) -> void;
        auto write(std::string_view name, const MeshData &value) -> void;

    private:
        std::vector<std::byte> _buffer;
    };

}
