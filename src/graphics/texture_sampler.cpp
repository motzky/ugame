#include "graphics/texture_sampler.h"

#include "graphics/opengl.h"
#include "utils/auto_release.h"

namespace game
{
    TextureSampler::TextureSampler()
        : _handle{0u, [](auto sampler)
                  { ::glDeleteSamplers(1, &sampler); }}
    {
        ::glCreateSamplers(1, &_handle);

        ::glSamplerParamerteri(_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        ::glSamplerParamerteri(_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        ::glSamplerParamerterf(_handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.f);
    }

    auto TextureSampler::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}