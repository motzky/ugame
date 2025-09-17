#include <gtest/gtest.h>

#include <cmath>
#include <numbers>
#include <print>

#include "graphics/frustum_plane.h"
#include "math/vector3.h"

TEST(frustum_plane, ctor_two_vectors)
{
    const auto plane = game::FrustumPlane{{1.f, 2.f, 3.f}, {0.f, 0.f, 1.f}};
    const auto expected = game::Vector3::normalize(game::Vector3{0.f, 0.f, 1.f});

    EXPECT_NEAR(plane.normal.x, expected.x, 0.0001f);
    EXPECT_NEAR(plane.normal.y, expected.y, 0.0001f);
    EXPECT_NEAR(plane.normal.z, expected.z, 0.0001f);
    EXPECT_NEAR(plane.distance, 3.f, 0.001f);
}

TEST(frustum_plane, ctor_four_floats)
{
    const auto plane = game::FrustumPlane{1.f, 2.f, 3.f, 4.f};
    const auto expected = game::Vector3::normalize(game::Vector3{1.f, 2.f, 3.f});

    EXPECT_NEAR(plane.normal.x, expected.x, 0.0001f);
    EXPECT_NEAR(plane.normal.y, expected.y, 0.0001f);
    EXPECT_NEAR(plane.normal.z, expected.z, 0.0001f);
    EXPECT_NEAR(plane.distance, 4.f / std::hypot(1.f, 2.f, 3.f), 0.001f);
}

TEST(frustum_plane, intesection_at_origin)
{
    const auto xy = game::FrustumPlane{{1.f, 1.f, 0.f}, {0.f, 0.f, 1.f}};
    const auto xz = game::FrustumPlane{{1.f, 0.f, 1.f}, {0.f, 1.f, 0.f}};
    const auto yz = game::FrustumPlane{{0.f, 1.f, 1.f}, {1.f, 0.f, 0.f}};

    auto v = game::intersection(xy, xz, yz);

    EXPECT_NEAR(v.x, 0.f, 0.00001f);
    EXPECT_NEAR(v.y, 0.f, 0.00001f);
    EXPECT_NEAR(v.z, 0.f, 0.00001f);
}

TEST(frustum_plane, intersection_at_one_one_one)
{
    const auto xy = game::FrustumPlane{game::Vector3{1.f, 1.f, 0.f} + game::Vector3{1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}};
    const auto xz = game::FrustumPlane{game::Vector3{1.f, 0.f, 1.f} + game::Vector3{1.f, 1.f, 1.f}, {0.f, 1.f, 0.f}};
    const auto yz = game::FrustumPlane{game::Vector3{0.f, 1.f, 1.f} + game::Vector3{1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}};

    auto v = game::intersection(xy, xz, yz);

    EXPECT_NEAR(v.x, -1.f, 0.00001f);
    EXPECT_NEAR(v.y, -1.f, 0.00001f);
    EXPECT_NEAR(v.z, -1.f, 0.00001f);
}

TEST(frustum_plane, intersection_at_minus_two_zero_three)
{
    const auto xy = game::FrustumPlane{game::Vector3{1.f, 1.f, 0.f} + game::Vector3{-2.f, 0.f, 3.f}, {0.f, 0.f, 1.f}};
    const auto xz = game::FrustumPlane{game::Vector3{1.f, 0.f, 1.f} + game::Vector3{-2.f, 0.f, 3.f}, {0.f, 1.f, 0.f}};
    const auto yz = game::FrustumPlane{game::Vector3{0.f, 1.f, 1.f} + game::Vector3{-2.f, 0.f, 3.f}, {1.f, 0.f, 0.f}};

    auto v = game::intersection(xy, xz, yz);

    EXPECT_NEAR(v.x, 2.f, 0.00001f);
    EXPECT_NEAR(v.y, 0.f, 0.00001f);
    EXPECT_NEAR(v.z, -3.f, 0.00001f);
}
