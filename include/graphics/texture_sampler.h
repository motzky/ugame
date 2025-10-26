#pragma once

#include <optional>
#include <string>

#include "opengl.h"
#include "utils/auto_release.h"

namespace game
{
    enum class FilterType
    {
        LINEAR_MIPMAP,
        LINEAR,
        NEAREST
    };

    class TextureSampler
    {
    public:
        TextureSampler(FilterType min_filter, FilterType mag_filter, std::optional<float> ansisotropic_samples = std::nullopt);

        auto native_handle() const -> ::GLuint;

    private:
        AutoRelease<::GLuint> _handle;
    };

    auto to_string(FilterType filter_type) -> std::string;
}