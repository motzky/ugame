#pragma once

#include <cstdint>

#include "events/key_event.h"
#include "graphics/buffer.h"
#include "graphics/opengl.h"
#include "loaders/mesh_loader.h"
#include "utils/auto_release.h"

namespace game
{

    class TlvReader;

    class Mesh
    {
    public:
        Mesh(MeshData data);
        Mesh(const TlvReader &reader, std::string_view name);

        auto bind() const -> void;
        auto unbind() const -> void;
        auto index_count() const -> std::uint32_t;
        auto index_offset() const -> std::uintptr_t;

        auto mesh_data() const -> MeshData;

    private:
        AutoRelease<GLuint> _vao;
        Buffer _vbo;

        std::uint32_t _index_count;
        std::uintptr_t _index_offset;
        MeshData _meshData;
    };

}