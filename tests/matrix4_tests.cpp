#include <gtest/gtest.h>

#include <numbers>
#include <print>

#include "math_primitives/matrix4.h"
#include "math_primitives/vector3.h"

TEST(matrx4, multiply_by_identity)
{
    const auto m1 = game::Matrix4{{
        1.f,
        2.f,
        3.f,
        4.f,
        1.f,
        2.f,
        3.f,
        4.f,
        1.f,
        2.f,
        3.f,
        4.f,
        1.f,
        2.f,
        3.f,
        4.f,
    }};
    const auto m2 = game::Matrix4{};

    ASSERT_EQ(m1 * m2, m1);
}

TEST(matrix4, look_at)
{
    const auto view = game::Matrix4::look_at({1.f, 0.f, 5.f}, {0.f}, {0.f, 1.f, 0.f});
    const auto expected = game::Matrix4{{0.980581f, 0.f, 0.196116f, 0.f,
                                         0.f, 1.f, 0.f, 0.f, -0.196116f, 0.f, 0.980581f, 0.f,
                                         0.f, 0.f, -5.099020f, 1.f}};

    const auto view_spn = view.data();
    const auto exp_spn = expected.data();
    for (auto i = 0u; i < 16u; i++)
    {
        ASSERT_NEAR(view_spn[i], exp_spn[i], 0.00001f);
    }
}

TEST(matrix4, perspective)
{
    const auto proj = game::Matrix4::perspective(std::numbers::pi_v<float> / 4.f, 800.f, 600.f, 0.1f, 100.f);
    const auto expected = game::Matrix4{{1.81066f, 0.f, 0.f, 0.f,
                                         0.f, 2.414213f, 0.f, 0.f,
                                         0.f, 0.f, -1.002002f, -1.f,
                                         0.f, 0.f, -0.200200f, 0.f}};

    const auto view_spn = proj.data();
    const auto exp_spn = expected.data();
    for (auto i = 0u; i < 16u; i++)
    {
        ASSERT_NEAR(view_spn[i], exp_spn[i], 0.00001f);
    }
}