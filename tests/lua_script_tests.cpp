#include <gtest/gtest.h>

#include "core/exception.h"
#include "scripting/lua_script.h"

TEST(lua_script, simple_script)
{
    ::testing::internal::CaptureStdout();

    auto script = game::LuaScript{R"(print("hello world"))"};

    const auto output = ::testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "hello world\n");
}

TEST(lua_script, bad_script)
{
    EXPECT_THROW(game::LuaScript{R"(print("hello world)"}, game::Exception);
}
