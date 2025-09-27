#include <gtest/gtest.h>

#include <string>
#include <string_view>

#include "core/exception.h"
#include "utils/ensure.h"

using namespace std::literals;

TEST(exception, simple)
{
    const auto ex = game::Exception("hello");

    ASSERT_EQ(ex.what(), "hello"sv);
}

TEST(ensure, expect_death)
{
    ASSERT_DEATH({ game::expect(false, "something bad"); }, "");
}

TEST(ensure, ensure_fail)
{
    ASSERT_THROW({ game::ensure(false, "something bad happened"); }, game::Exception);
}
