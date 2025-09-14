#include <gtest/gtest.h>

#include "math/vector4.h"

TEST(vector4, emtpy_ctor)
{
    const auto v = game::Vector4{};
    ASSERT_FLOAT_EQ(v.x, 0.f);
    ASSERT_FLOAT_EQ(v.y, 0.f);
    ASSERT_FLOAT_EQ(v.z, 0.f);
}

TEST(vector4, single_value_ctor)
{
    const auto v = game::Vector4{1.1f};
    ASSERT_FLOAT_EQ(v.x, 1.1f);
    ASSERT_FLOAT_EQ(v.y, 1.1f);
    ASSERT_FLOAT_EQ(v.z, 1.1f);
}

TEST(vector4, all_components_ctor)
{
    const auto v = game::Vector4{1.1f, 2.2f, 3.3f, 4.4f};
    ASSERT_FLOAT_EQ(v.x, 1.1f);
    ASSERT_FLOAT_EQ(v.y, 2.2f);
    ASSERT_FLOAT_EQ(v.z, 3.3f);
    ASSERT_FLOAT_EQ(v.w, 4.4f);
}

TEST(vector4, negate)
{
    const auto v1 = game::Vector4{1.f, 2.f, 3.f, 4.f};
    const auto expected = game::Vector4{-1.f, -2.f, -3.f, -4.f};

    ASSERT_EQ(-v1, expected);
}

TEST(vector4, normalize_point)
{
    auto v1 = game::Vector4{};
    auto norm = game::Vector4::normalize(v1);

    const auto expected = game::Vector4{};

    ASSERT_EQ(norm, expected);
}

TEST(vector4, add_zero)
{
    auto v1 = game::Vector4{1.f, 2.f, 3.f, 4.f};
    const auto zero = game::Vector4{};
    const auto expected = game::Vector4{1.f, 2.f, 3.f, 4.f};

    v1 += zero;

    ASSERT_EQ(v1, expected);
}

TEST(vector4, subtract_negated)
{
    const auto v1 = game::Vector4{1.f, 2.f, 3.f, 4.f};
    const auto v2 = v1 + (-v1);
    const auto zero = game::Vector4{};

    ASSERT_EQ(v2, zero);
}

TEST(vector4, subtract)
{
    const auto v1 = game::Vector4{10.f, 20.f, 30.f, 40.f};
    const auto v2 = game::Vector4{4.f, 5.f, 6.4f, 7.5f};
    const auto expected = game::Vector4{6.f, 15.f, 23.6f, 32.5f};

    auto v3 = v1 - v2;

    ASSERT_EQ(v3.x, expected.x);
    ASSERT_EQ(v3.y, expected.y);
    ASSERT_EQ(v3.z, expected.z);
}