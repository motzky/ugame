#pragma once

#include <span>
#include <string>
#include <string_view>
#include <tuple>

#include "cube_map.h"
#include "graphics/shader.h"
#include "math/matrix4.h"
#include "opengl.h"
#include "texture.h"
#include "texture_sampler.h"
#include "utils/auto_release.h"
#include "utils/string_unordered_map.h"

namespace game
{
    class Material
    {
    public:
        Material(const Shader &vertex_shader, const Shader &fragment_shader);

        auto use() const -> void;
        auto set_uniform(std::string_view name, const Matrix4 &obj) const -> void;
        auto set_uniform(std::string_view name, std::int32_t obj) const -> void;
        auto set_uniform(std::string_view name, float obj) const -> void;
        auto bind_cube_map(const CubeMap *texture, const TextureSampler *sampler) const -> void;
        auto bind_texture(std::uint32_t index, const Texture *texture, const TextureSampler *sampler) const -> void;
        auto bind_textures(std::span<const std::tuple<const Texture *, const TextureSampler *>>) const -> void;

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<::GLuint> _handle;
        StringUnorderedMap<::GLuint> _uniforms;
    };
}
