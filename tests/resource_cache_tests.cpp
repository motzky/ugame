#include <gtest/gtest.h>

#include "resources/resource_cache.h"

TEST(resource_cache, insert_int)
{
    auto cache = game::ResourceCache<int>{};

    const auto *v = cache.insert<int>("test_int", 12345);

    ASSERT_EQ(*v, 12345);
}

TEST(resource_cache, get_int)
{
    auto cache = game::ResourceCache<int>{};

    cache.insert<int>("test_int", 12345);

    const auto *v = cache.get<int>("test_int");

    ASSERT_EQ(*v, 12345);
}
