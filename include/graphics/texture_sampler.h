#pragma once

#include "auto_release.h"
#include "opengl.h"

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