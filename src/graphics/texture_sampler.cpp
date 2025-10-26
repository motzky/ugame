#include "graphics/texture_sampler.h"

#include <optional>
#include <string>

#include "graphics/opengl.h"
#include "utils/auto_release.h"
#include "utils/ensure.h"
#include "utils/exception.h"
#include "utils/formatter.h"

namespace
{
    auto to_opengl(game::FilterType filter_type) -> ::GLenum
    {
        switch (filter_type)
        {
            using enum game::FilterType;
        case LINEAR_MIPMAP:
            return GL_LINEAR_MIPMAP_LINEAR;
        case LINEAR:
            return GL_LINEAR;
        case NEAREST:
            return GL_NEAREST;
        }

        throw game::Exception("unknown filter type: {}", filter_type);
    }
}

namespace game
{
    TextureSampler::TextureSampler(FilterType min_filter, FilterType mag_filter, std::optional<float> ansisotropic_samples)
        : _handle{0u, [](auto sampler)
                  { ::glDeleteSamplers(1, &sampler); }}
    {
        ::glCreateSamplers(1, &_handle);

        ::glSamplerParamerteri(_handle, GL_TEXTURE_MIN_FILTER, to_opengl(min_filter));
        ::glSamplerParamerteri(_handle, GL_TEXTURE_MAG_FILTER, to_opengl(mag_filter));
        if (ansisotropic_samples)
        {
            expect(*ansisotropic_samples >= 1.f, "invalid samples: {}", *ansisotropic_samples);
            ::glSamplerParamerterf(_handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, *ansisotropic_samples);
        }
    }

    auto TextureSampler::native_handle() const -> ::GLuint
    {
        return _handle;
    }

    auto to_string(FilterType filter_type) -> std::string
    {
        switch (filter_type)
        {
            using enum game::FilterType;
        case LINEAR_MIPMAP:
            return "LINEAR_MIPMAP_LINEAR";
        case LINEAR:
            return "LINEAR";
        case NEAREST:
            return "NEAREST";
        default:
            return "<unknown>";
        }
    }

}