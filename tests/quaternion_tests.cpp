#include <gtest/gtest.h>

#include <cmath>
#include <numbers>
#include <print>

#include "math/quaternion.h"

TEST(quaternion, multiply)
{
    auto angle = std::numbers::pi_v<float> / 2.f;

    auto q1 = game::Quaternion(std::sin(angle / 2.f), 0.f, 0.f, std::cos(angle / 2.f));
    auto q2 = game::Quaternion(0.f, std::sin(angle / 2.f), 0.f, std::cos(angle / 2.f));

    auto result = q1 * q2;

    auto expected = game::Quaternion(.5f, .5f, .5f, .5f);
    ASSERT_FLOAT_EQ(result.x, expected.x);
    ASSERT_FLOAT_EQ(result.y, expected.y);
    ASSERT_FLOAT_EQ(result.z, expected.z);
    ASSERT_FLOAT_EQ(result.w, expected.w);
}

TEST(quaternion, multiply_inv)
{
    auto angle = std::numbers::pi_v<float> / 2.f;

    auto q1 = game::Quaternion(std::sin(angle / 2.f), 0.f, 0.f, std::cos(angle / 2.f));
    auto q2 = game::Quaternion(0.f, std::sin(angle / 2.f), 0.f, std::cos(angle / 2.f));

    auto result = q2 * q1;

    auto expected = game::Quaternion(.5f, .5f, -.5f, .5f);
    ASSERT_FLOAT_EQ(result.x, expected.x);
    ASSERT_FLOAT_EQ(result.y, expected.y);
    ASSERT_FLOAT_EQ(result.z, expected.z);
    ASSERT_FLOAT_EQ(result.w, expected.w);
}