#include <gtest/gtest.h>

#include "vector3.h"

TEST(vector3, negate)
{
    const auto v1 = game::Vector3{1.f, 2.f, 3.f};
    const auto expected = game::Vector3{-1.f, -2.f, -3.f};

    ASSERT_EQ(-v1, expected);
}

TEST(vector3, add_zero)
{
    auto v1 = game::Vector3{1.f, 2.f, 3.f};
    const auto zero = game::Vector3{0.f, 0.f, 0.f};
    const auto expected = game::Vector3{1.f, 2.f, 3.f};

    v1 += zero;

    ASSERT_EQ(v1, expected);
}

TEST(vector3, normalize)
{
    auto v1 = game::Vector3{1.f, 2.f, 3.f};
    auto norm = game::Vector3::normalize(v1);

    const auto expected = game::Vector3{0.26726124f, 0.5345225f, 0.80178374};

    ASSERT_EQ(norm, expected);
}

TEST(vector3, multiply_assign_scalar)
{
    auto v1 = game::Vector3{1.f, 2.f, 3.f};

    const auto expected = game::Vector3{10.f, 20.f, 30.f};

    v1 *= 10.f;

    ASSERT_EQ(v1, expected);
}