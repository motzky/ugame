#pragma once

#include <cstdint>
#include <memory>

namespace game
{
    class Settings
    {
    public:
        Settings(Settings &) = delete;
        void operator=(const Settings &) = delete;
        Settings(Settings &&) = delete;
        void operator=(const Settings &&) = delete;

        static auto instance() -> Settings &;
        static auto destroy() -> void;

        auto width() const -> std::uint32_t;
        auto height() const -> std::uint32_t;
        auto anti_aliasing() const -> bool;
        auto msaa_samples() const -> std::uint8_t;

        auto set_width(std::uint32_t value) -> void;
        auto set_height(std::uint32_t value) -> void;
        auto set_anti_aliasing(bool enable) -> void;
        auto set_msaa_samples(std::uint8_t samples) -> void;

    private:
        Settings();

        static std::unique_ptr<Settings> _instance;
        std::uint32_t _width;
        std::uint32_t _height;
        bool _anti_aliasing;
        std::uint8_t _samples;
    };
}