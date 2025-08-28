#include "material.h"

#include "auto_release.h"
#include "error.h"
#include "opengl.h"
#include "shader.h"

namespace game
{
    Material::Material(const Shader &vertex_shader, const Shader &fragment_shader)
        : _handle{}
    {
        ensure(vertex_shader.type() == ShaderType::VERTEX, "vertex_shader is not a vertex shader");
        ensure(fragment_shader.type() == ShaderType::FRAGMENT, "fragment_shader is not a fragment shader");

        _handle = game::AutoRelease<::GLuint>{::glCreateProgram(), ::glDeleteProgram};
        game::ensure(_handle, "failed ot create opengl program");

        ::glAttachShader(_handle, vertex_shader.native_handle());
        ::glAttachShader(_handle, fragment_shader.native_handle());
        ::glLinkProgram(_handle);
    }

    auto Material::native_handle() const -> ::GLuint
    {
        return _handle;
    }

}