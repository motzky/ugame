#include <gtest/gtest.h>

#include "vector3.h"

TEST(vector3, negate)
{
    const auto v1 = game::Vector3{.x = 1.f, .y = 2.f, .z = 3.f};
    const auto expected = game::Vector3{.x = -1.f, .y = -2.f, .z = -3.f};

    ASSERT_EQ(-v1, expected);
}
