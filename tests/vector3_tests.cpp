#include <gtest/gtest.h>

#include "vector3.h"

TEST(vector3, negate)
{
    const auto v1 = game::Vector3{.x = 1.f, .y = 2.f, .z = 3.f};
    const auto expected = game::Vector3{.x = -1.f, .y = -2.f, .z = -3.f};

    ASSERT_EQ(-v1, expected);
}

TEST(vector3, add_zero)
{
    auto v1 = game::Vector3{.x = 1.f, .y = 2.f, .z = 3.f};
    const auto zero = game::Vector3{.x = 0.f, .y = 0.f, .z = 0.f};
    const auto expected = game::Vector3{.x = 1.f, .y = 2.f, .z = 3.f};

    v1 += zero;

    ASSERT_EQ(v1, expected);
}

TEST(vector3, normalize)
{
    auto v1 = game::Vector3{.x = 1.f, .y = 2.f, .z = 3.f};
    auto norm = game::Vector3::normalize(v1);

    const auto expected = game::Vector3{.x = 0.26726124f, .y = 0.5345225f, .z = 0.80178374};

    ASSERT_EQ(norm, expected);
}