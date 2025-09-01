#pragma once

#include <cstdint>

#include "auto_release.h"
#include "opengl.h"

namespace game
{

    class Mesh
    {
    public:
        Mesh();

        auto bind() const -> void;
        auto unbind() const -> void;
        auto index_count() const -> std::uint32_t;
        auto index_offset() const -> std::uintptr_t;

    private:
        AutoRelease<GLuint> _vao;
        AutoRelease<GLuint> _vbo;

        std::uint32_t _index_count;
        std::uintptr_t _index_offset;
    };

}