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
    }

    auto TextureSampler::native_handle() const -> ::GLuint
    {
        return _handle;
    }
}