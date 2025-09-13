#include <gtest/gtest.h>

#include "game/static_object_transformer.h"
#include "math/vector3.h"

TEST(static_object_transformer, position)
{
    const auto position = game::Vector3{1.f, 2.f, 3.f};
    const auto tranformer = game::StaticObjectTransformer{position};

    ASSERT_EQ(tranformer.position(), position);
}

TEST(static_object_transformer, update_does_nothing)
{
    const auto position = game::Vector3{1.f, 2.f, 3.f};
    auto tranformer = game::StaticObjectTransformer{position};

    tranformer.update();

    ASSERT_EQ(tranformer.position(), position);
}
