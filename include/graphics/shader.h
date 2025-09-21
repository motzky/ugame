#pragma once

#include <format>
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

}

template <>
struct std::formatter<game::ShaderType>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::ShaderType &obj, std::format_context &ctx) const
    {
        switch (obj)
        {
            using enum game::ShaderType;
        case VERTEX:
            return std::format_to(ctx.out(), "VERTEX");
        case FRAGMENT:
            return std::format_to(ctx.out(), "FRAGMENT");
        case COMPUTE:
            return std::format_to(ctx.out(), "COMPUTE");
        }

        throw game::Exception("unknown shader type: {}", std::to_underlying(obj));
    }
};
