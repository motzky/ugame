#include <numbers>
#include <print>

#include <gtest/gtest.h>

#include "graphics/camera.h"

TEST(camera, simple)
{
    auto camera = game::Camera{{0.f, 0.f, 0.f},
                               {0.f, 0.f, -1.f},
                               {0.f, 1.f, 0.f},
                               std::numbers::pi_v<float> / 4.f,
                               1920.f,
                               1080.f,
                               0.1f,
                               100.f};

    const auto planes = camera.frustum_planes();
    const auto &[far, near, right, left, top, bottom] = planes;

    EXPECT_NEAR(near.normal.x, 0.f, 0.00001f);
    EXPECT_NEAR(near.normal.y, 0.f, 0.00001f);
    EXPECT_NEAR(near.normal.z, -1.f, 0.00001f);
    EXPECT_NEAR(near.distance, -.1f, 0.00001f);

    EXPECT_NEAR(far.normal.x, 0.f, 0.00001f);
    EXPECT_NEAR(far.normal.y, 0.f, 0.00001f);
    EXPECT_NEAR(far.normal.z, 1.f, 0.00001f);
    EXPECT_NEAR(far.distance, 100.f, 0.0001f);

    EXPECT_NEAR(right.normal.x, 0.805234f, 0.00001f);
    EXPECT_NEAR(right.normal.y, 0.f, 0.00001f);
    EXPECT_NEAR(right.normal.z, -0.592958f, 0.00001f);
    EXPECT_NEAR(right.distance, 0.f, 0.00001f);

    EXPECT_NEAR(left.normal.x, -.805234f, 0.00001f);
    EXPECT_NEAR(left.normal.y, 0.f, 0.00001f);
    EXPECT_NEAR(left.normal.z, -.592958f, 0.00001f);
    EXPECT_NEAR(left.distance, 0.f, 0.00001f);

    EXPECT_NEAR(top.normal.x, 0.f, 0.00001f);
    EXPECT_NEAR(top.normal.y, 0.923880f, 0.00001f);
    EXPECT_NEAR(top.normal.z, -.382683f, 0.00001f);
    EXPECT_NEAR(top.distance, 0.f, 0.00001f);

    EXPECT_NEAR(bottom.normal.x, 0.f, 0.00001f);
    EXPECT_NEAR(bottom.normal.y, -.923880f, 0.00001f);
    EXPECT_NEAR(bottom.normal.z, -.382683f, 0.00001f);
    EXPECT_NEAR(bottom.distance, 0.f, 0.00001f);
}