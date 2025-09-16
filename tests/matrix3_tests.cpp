#include <gtest/gtest.h>

#include <numbers>
#include <print>

#include "math/matrix3.h"
#include "math/vector3.h"

TEST(matrix3, ctor_identity)
{
    float vals[] = {
        1.f,
        0.f,
        0.f,
        0.f,
        1.f,
        0.f,
        0.f,
        0.f,
        1.f,
    };
    auto m = game::Matrix3{};

    ASSERT_FLOAT_EQ(m[0], vals[0]);
    ASSERT_FLOAT_EQ(m[1], vals[1]);
    ASSERT_FLOAT_EQ(m[2], vals[2]);
    ASSERT_FLOAT_EQ(m[3], vals[3]);
    ASSERT_FLOAT_EQ(m[4], vals[4]);
    ASSERT_FLOAT_EQ(m[5], vals[5]);
    ASSERT_FLOAT_EQ(m[6], vals[6]);
    ASSERT_FLOAT_EQ(m[7], vals[7]);
    ASSERT_FLOAT_EQ(m[8], vals[8]);
}

TEST(matrix3, multiply_by_identity)
{
    float vals[] = {
        1.f,
        2.f,
        3.f,
        1.f,
        2.f,
        3.f,
        1.f,
        2.f,
        3.f,
    };
    const auto m1 = game::Matrix3{vals};
    const auto m2 = game::Matrix3{};

    ASSERT_EQ(m1 * m2, m1);
}

TEST(matrix3, ctor_from_three_vectors)
{
    const auto v1 = game::Vector3{3.f, 2.f, 1.f};
    const auto v2 = game::Vector3{7.f, 8.f, 9.f};
    const auto v3 = game::Vector3{4.f, 5.f, 6.f};

    auto m = game::Matrix3(v1, v2, v3);

    ASSERT_FLOAT_EQ(m[0], 3.f);
    ASSERT_FLOAT_EQ(m[1], 2.f);
    ASSERT_FLOAT_EQ(m[2], 1.f);
    ASSERT_FLOAT_EQ(m[3], 7.f);
    ASSERT_FLOAT_EQ(m[4], 8.f);
    ASSERT_FLOAT_EQ(m[5], 9.f);
    ASSERT_FLOAT_EQ(m[6], 4.f);
    ASSERT_FLOAT_EQ(m[7], 5.f);
    ASSERT_FLOAT_EQ(m[8], 6.f);
}

TEST(matrix3, multiply_with_vector)
{
    const auto v1 = game::Vector3{1.f, 2.f, 3.f};
    const auto v2 = game::Vector3{4.f, 5.f, 6.f};
    const auto v3 = game::Vector3{7.f, 8.f, 9.f};

    auto m = game::Matrix3(v1, v2, v3);
    const auto v4 = game::Vector3{10.f, 10.f, 10.f};

    const auto mult = m * v4;

    ASSERT_FLOAT_EQ(mult.x, 120.f);
    ASSERT_FLOAT_EQ(mult.y, 150.f);
    ASSERT_FLOAT_EQ(mult.z, 180.f);
}
