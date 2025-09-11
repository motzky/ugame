#pragma once

#include "opengl.h"
#include "utils/auto_release.h"

namespace game
{
    class TextureSampler
    {
    public:
        TextureSampler();

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<::GLuint> _handle;
    };
}