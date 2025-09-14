#include <gtest/gtest.h>
#include <numbers>

#include "camera.h"
#include "game/inverse_camera_object_transformer.h"
#include "math/vector3.h"

TEST(inverse_camer_object_transformer, position)
{
    auto camera = game::Camera{{0.f, 2.f, 20.f},
                               {0.f, 0.f, 0.f},
                               {0.f, 1.f, 0.f},
                               std::numbers::pi_v<float> / 4.f,
                               static_cast<float>(1920),
                               static_cast<float>(1080),
                               0.001f,
                               100.f};

    const auto position = game::Vector3{1.f, 2.f, 3.f};
    const auto tranformer = game::InverseCameraObjectTransformer{position, camera};

    ASSERT_EQ(tranformer.position(), position);
}

TEST(inverse_camera_object_transformer, update_camera_did_not_move)
{
    auto camera = game::Camera{{0.f, 2.f, 20.f},
                               {0.f, 0.f, 0.f},
                               {0.f, 1.f, 0.f},
                               std::numbers::pi_v<float> / 4.f,
                               static_cast<float>(1920),
                               static_cast<float>(1080),
                               0.001f,
                               100.f};

    const auto position = game::Vector3{0.f, 2.f, 20.f};
    auto tranformer = game::InverseCameraObjectTransformer{camera.position(), camera};

    tranformer.update();

    ASSERT_EQ(tranformer.position(), position);
}

TEST(inverse_camera_object_transformer, update_camera_did_move)
{
    auto camera = game::Camera{{0.f, 2.f, 20.f},
                               {0.f, 0.f, 0.f},
                               {0.f, 1.f, 0.f},
                               std::numbers::pi_v<float> / 4.f,
                               static_cast<float>(1920),
                               static_cast<float>(1080),
                               0.001f,
                               100.f};

    auto tranformer = game::InverseCameraObjectTransformer{camera.position(), camera};

    const auto expected = game::Vector3{-1.f, 2.f, 22.f};

    camera.translate({1.f, 0.f, -2.f});

    tranformer.update();

    auto new_pos = tranformer.position();
    ASSERT_FLOAT_EQ(new_pos.x, expected.x);
    ASSERT_FLOAT_EQ(new_pos.y, expected.y);
    ASSERT_FLOAT_EQ(new_pos.z, expected.z);
}
