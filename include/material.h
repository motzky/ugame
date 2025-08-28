#pragma once

#include "auto_release.h"
#include "opengl.h"
#include "shader.h"

namespace game
{
    class Material
    {
    public:
        Material(const Shader &vertex_shader, const Shader &fragment_shader);

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<::GLuint> _handle;
    };
}
