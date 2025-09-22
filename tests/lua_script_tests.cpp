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

TEST(lua_script, simple_function_call)
{

    auto script = game::LuaScript{R"(
function hello()
    print ("hello world")
end
        )"};

    script.set_function("hello");
    ::testing::internal::CaptureStdout();

    script.execute(0u, 0u);

    const auto output = ::testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "hello world\n");
}

TEST(lua_script, missing_function_call)
{
    auto script = game::LuaScript{R"(
function hello()
        print("hello world")
end)"};

    ASSERT_THROW(script.set_function("hell"), game::Exception);
}

TEST(lua_script, bad_function_call)
{
    auto script = game::LuaScript{R"(
function hello()
        foo()
end)"};

    script.set_function("hello");

    ASSERT_THROW(script.execute(0u, 0u), game::Exception);
}

TEST(lua_script, function_call_single_arg)
{
    auto script = game::LuaScript{R"(
function my_print(msg)
        print(msg)
end)"};

    script.set_function("my_print");
    script.set_argument("hello world");
    ::testing::internal::CaptureStdout();
    script.execute(1u, 0u);
    const auto output = ::testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "hello world\n");
}

TEST(lua_script, function_call_get_no_arg)
{
    auto script = game::LuaScript{R"(
function get_num()
end)"};

    script.set_function("get_num");
    script.execute(0u, 1u);
    auto res = std::int64_t{};
    ASSERT_THROW(script.get_result(res), game::Exception);
}
