#pragma once

#include "auto_release.h"
#include "opengl.h"

namespace game
{

    class Mesh
    {
    public:
        Mesh();

        auto bind() -> void;
        auto unbind() -> void;

    private:
        AutoRelease<GLuint> _vao;
        AutoRelease<GLuint> _vbo;
    };

}