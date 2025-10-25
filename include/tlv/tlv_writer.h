#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "graphics/mesh_data.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"
#include "sound/sound_data.h"
#include "tlv/tlv_entry.h"

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
        auto write(std::string_view name, std::string_view value) -> void;
        auto write(std::string_view name, std::span<const std::string> sub_mesh_names) -> void;
        auto write(std::string_view name, const SoundData &data) -> void;

    private:
        std::vector<std::byte> _buffer;
    };

}
