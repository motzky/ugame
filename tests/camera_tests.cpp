#include <numbers>
#include <print>

#include <gtest/gtest.h>

#include "camera.h"

TEST(camera, simple)
{
    auto camera = game::Camera{{0.f, 10.f, 0.f},
                               {0.f, 0.f, -1.f},
                               {0.f, 1.f, 0.f},
                               std::numbers::pi_v<float> / 4.f,
                               1920.f,
                               1080.f,
                               0.1f,
                               100.f};

    const auto planes = camera.calculate_frustum_planes();
    for (const auto &plane : planes)
    {
        std::println("{}", plane);
    }

    // TODO Make real test
    ASSERT_TRUE(true);
}
