#include "shader.h"

#include "error.h"
#include "exception.h"

namespace
{
    auto to_native(game::ShaderType type) -> ::GLenum
    {
        switch (type)
        {
            using enum game::ShaderType;
        case VERTEX:
            return GL_VERTEX_SHADER;
        case FRAGMENT:
            return GL_FRAGMENT_SHADER;
        case COMPUTE:
            return GL_COMPUTE_SHADER;
        }
        throw game::Exception("unknown shader type: {}", std::to_underlying(type));
    }
}

namespace game
{
    Shader::Shader(std::string_view source, ShaderType type)
        : _handle{}, _type(type)
    {
        _handle = AutoRelease<::GLuint>{::glCreateShader(to_native(type)), ::glDeleteShader};

        const ::GLchar *strings[] = {source.data()};
        const ::GLint lengths[] = {static_cast<::GLint>(source.length())};

        ::glShaderSource(_handle, 1, strings, lengths);
        ::glCompileShader(_handle);

        ::GLint result{};
        ::glGetShaderiv(_handle, GL_COMPILE_STATUS, &result);
        game::ensure(result == GL_TRUE, "failed to compile shader {}", _type);
    }

    auto Shader::type() const -> ShaderType
    {
        return _type;
    }

    auto Shader::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}
