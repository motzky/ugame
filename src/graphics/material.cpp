#include "graphics/material.h"

#include <format>
#include <ranges>
#include <span>
#include <string>
#include <string_view>

#include "graphics/opengl.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "log.h"
#include "utils/auto_release.h"
#include "utils/ensure.h"

namespace game
{
    Material::Material(std::string_view vertex_shader_name, const Shader &vertex_shader, std::string_view fragment_shader_name, const Shader &fragment_shader)
        : _handle{},
          _uniforms{},
          _uniform_callback{},
          _vertex_shader_name{vertex_shader_name},
          _fragment_shader_name{fragment_shader_name}
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

        auto uniform_count = ::GLint{};
        ::glGetProgramiv(_handle, GL_ACTIVE_UNIFORMS, &uniform_count);

        if (uniform_count == 0)
        {
            return;
        }

        auto max_name_length = ::GLint{};
        ::glGetProgramiv(_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);

        auto length = ::GLsizei{};
        auto count = ::GLsizei{};
        auto type = ::GLenum{};

        for (auto i = ::GLint{}; i < uniform_count; ++i)
        {
            auto name = std::string(max_name_length, '\0');

            ::glGetActiveUniform(_handle, i, max_name_length, &length, &count, &type, name.data());
            name.resize(length);

            const auto location = ::glGetUniformLocation(_handle, name.c_str());
            _uniforms[name] = location;
        }
    }

    auto Material::use() const -> void
    {
        ::glUseProgram(_handle);
    }

    auto Material::set_uniform(std::string_view name, const Color &obj) const -> void
    {
        const auto uniform = _uniforms.find(name);
        ensure(uniform != std::ranges::cend(_uniforms), "uniform not found: {} in material {}, {}", name, _vertex_shader_name, _fragment_shader_name);

        ::glUniform3fv(uniform->second, 1, reinterpret_cast<const ::GLfloat *>(std::addressof(obj)));
    }

    auto Material::set_uniform(std::string_view name, const Matrix4 &obj) const -> void
    {
        const auto uniform = _uniforms.find(name);
        ensure(uniform != std::ranges::cend(_uniforms), "uniform not found: {} in material {}, {}", name, _vertex_shader_name, _fragment_shader_name);

        ::glUniformMatrix4fv(uniform->second, 1, GL_FALSE, obj.data().data());
    }

    auto Material::set_uniform(std::string_view name, std::int32_t obj) const -> void
    {
        const auto uniform = _uniforms.find(name);
        ensure(uniform != std::ranges::cend(_uniforms), "uniform not found: {} in material {}, {}", name, _vertex_shader_name, _fragment_shader_name);

        ::glUniform1i(uniform->second, obj);
    }

    auto Material::set_uniform(std::string_view name, float obj) const -> void
    {
        const auto uniform = _uniforms.find(name);
        ensure(uniform != std::ranges::cend(_uniforms), "uniform not found: {} in material {}, {}", name, _vertex_shader_name, _fragment_shader_name);

        ::glUniform1f(uniform->second, obj);
    }

    auto Material::bind_cube_map(const CubeMap *texture, const TextureSampler *sampler) const -> void
    {
        ::glBindTextureUnit(0, texture->native_handle());
        ::glBindSampler(0, sampler->native_handle());

        const auto uniform_name = std::format("tex{}", 0);
        set_uniform(uniform_name, 0);
    }

    auto Material::bind_texture(std::uint32_t index, const Texture *texture) const -> void
    {
        bind_texture(index, texture, texture->sampler());
    }

    auto Material::bind_texture(std::uint32_t index, const Texture *texture, const TextureSampler *sampler) const -> void
    {
        const auto gl_index = static_cast<::GLuint>(index);
        ::glBindTextureUnit(gl_index, texture->native_handle());
        ::glBindSampler(gl_index, sampler->native_handle());

        const auto uniform_name = std::format("tex{}", index);
        set_uniform(uniform_name, static_cast<std::int32_t>(index));
    }

    auto Material::bind_textures(std::span<const Texture *const> textures) const -> void
    {
        for (const auto &[index, tex] : textures | std::views::enumerate)
        {
            bind_texture(static_cast<uint32_t>(index), tex, tex->sampler());
        }
    }

    auto Material::set_uniform_callback(UniformCallback callback) -> void
    {
        _uniform_callback = std::move(callback);
    }

    auto Material::invoke_uniform_callback(const Entity *entity) const -> void
    {
        if (_uniform_callback)
        {
            _uniform_callback(this, entity);
        }
    }

    auto Material::native_handle() const -> ::GLuint
    {
        return _handle;
    }

}