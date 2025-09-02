#include "material.h"

#include "auto_release.h"
#include "ensure.h"
#include "opengl.h"
#include "shader.h"

namespace game
{
    Material::Material(const Shader &vertex_shader, const Shader &fragment_shader)
        : _handle{}
    {
        ensure(vertex_shader.type() == ShaderType::VERTEX, "vertex_shader is not a vertex shader");
        ensure(fragment_shader.type() == ShaderType::FRAGMENT, "fragment_shader is not a fragment shader");

        _handle = AutoRelease<::GLuint>{::glCreateProgram(), ::glDeleteProgram};
        ensure(_handle, "failed ot create opengl program");

        ::glAttachShader(_handle, vertex_shader.native_handle());
        ::glAttachShader(_handle, fragment_shader.native_handle());
        ::glLinkProgram(_handle);

        ::GLint result{};
        ::glGetProgramiv(_handle, GL_LINK_STATUS, &result);

        if (result != GL_TRUE)
        {
            char log[1024];
            ::glGetProgramInfoLog(_handle, sizeof(log), nullptr, log);

            game::ensure(result == GL_TRUE, "failed to link program \n{}", log);
        }
    }

    auto Material::native_handle() const -> ::GLuint
    {
        return _handle;
    }

}