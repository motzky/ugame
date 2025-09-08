#pragma once

#include <cstdint>

#include "auto_release.h"
#include "buffer.h"
#include "mesh_loader.h"
#include "opengl.h"

namespace game
{

    class TlvReader;

    class Mesh
    {
    public:
        Mesh(const MeshData &data);
        Mesh(const TlvReader &reader, std::string_view name);

        auto bind() const -> void;
        auto unbind() const -> void;
        auto index_count() const -> std::uint32_t;
        auto index_offset() const -> std::uintptr_t;

    private:
        AutoRelease<GLuint> _vao;
        Buffer _vbo;

        std::uint32_t _index_count;
        std::uintptr_t _index_offset;
    };

}