#include <game/settings.h>

#include <cstdint>
#include <memory>

#include "utils/ensure.h"

namespace game
{
    Settings::Settings()
        : _width{1280},
          _height(720),
          _anti_aliasing(false),
          _samples(1),
          _anisotropic_filtering(true),
          _anisotropic_filter_samples(16)
    {
    }

    auto Settings::instance() -> Settings &
    {
        if (!_instance)
        {
            _instance = std::make_unique<Settings>();
        }
        return *_instance.get();
    }

    auto Settings::destroy() -> void
    {
        _instance.release();
    }

    auto Settings::width() const -> std::uint32_t
    {
        return _width;
    }

    auto Settings::set_width(std::uint32_t value) -> void
    {
        _width = value;
    }

    auto Settings::height() const -> std::uint32_t
    {
        return _height;
    }

    auto Settings::set_height(std::uint32_t value) -> void
    {
        _height = value;
    }

    auto Settings::anti_aliasing() const -> bool
    {
        return _anti_aliasing;
    }

    auto Settings::set_anti_aliasing(bool enable) -> void
    {
        _anti_aliasing = enable;
        if (_anti_aliasing)
        {
            _samples = 1;
        }
    }

    auto Settings::msaa_samples() const -> std::uint8_t
    {
        return _samples;
    }

    auto Settings::set_msaa_samples(std::uint8_t samples) -> void
    {
        expect(samples >= 1, "Invalid number of samples: {}", samples);

        _samples = samples;
    }

    auto Settings::anisotropic_filtering() const -> bool
    {
        return _anisotropic_filtering;
    }

    auto Settings::set_anisotropic_filtering(bool enable) -> void
    {
        _anisotropic_filtering = enable;
    }

    auto Settings::anisotropic_filter_samples() const -> std::uint8_t
    {
        return _anisotropic_filter_samples;
    }

    auto Settings::set_anisotropic_filter_samples(std::uint8_t samples) -> void
    {
        _anisotropic_filter_samples = samples;
    }

}