#pragma once

#include <string_view>
#include <utility>

#include "core/exception.h"
#include "graphics/opengl.h"
#include "utils/auto_release.h"
#include "utils/ensure.h"

namespace game
{
    enum class ShaderType
    {
        VERTEX,
        FRAGMENT,
        COMPUTE
    };

    class Shader
    {
    public:
        Shader(std::string_view source, ShaderType type);

        auto type() const -> ShaderType;
        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<::GLuint> _handle;
        ShaderType _type;
    };

    auto to_string(ShaderType obj) -> std::string;

}
