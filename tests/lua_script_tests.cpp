#include <gtest/gtest.h>

#include "math/vector3.h"
#include "scripting/lua_script.h"
#include "utils/exception.h"
#include "utils/formatter.h"

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

TEST(lua_script, has_function_when_function_exists)
{
    auto script = game::LuaScript{R"(
function Hello()
        print("hello world")
end)"};

    ASSERT_TRUE(script.has_function("Hello"));
}

TEST(lua_script, has_function_when_function_not_exists)
{
    auto script = game::LuaScript{R"(
function Hello()
        print("hello world")
end)"};

    ASSERT_FALSE(script.has_function("Hello2"));
}

TEST(lua_script, has_function_when_name_is_not_function)
{
    auto script = game::LuaScript{R"(
Hello = 1
function Hello_World()
        print("hello world")
end)"};

    ASSERT_FALSE(script.has_function("Hello"));
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

TEST(lua_script, function_call_get_single_arg)
{
    auto script = game::LuaScript{R"(
function get_num()
        return 42
end)"};

    script.set_function("get_num");
    script.execute(0u, 1u);
    auto res = std::int64_t{};
    script.get_result(res);

    ASSERT_EQ(res, 42);
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

TEST(lua_script, function_call_multiple_args_multiple_returns)
{
    auto script = game::LuaScript{R"(
function get_nums(val1, val2, val3)
        return val1 + 1, val2 + 2, val3 + 3
end)"};

    script.set_function("get_nums");
    script.set_argument(static_cast<std::int64_t>(1));
    script.set_argument(static_cast<std::int64_t>(2));
    script.set_argument(static_cast<std::int64_t>(3));
    script.execute(3u, 3u);
    auto res1 = std::int64_t{};
    script.get_result(res1);
    auto res2 = std::int64_t{};
    script.get_result(res2);
    auto res3 = std::int64_t{};
    script.get_result(res3);

    ASSERT_EQ(res1, 6);
    ASSERT_EQ(res2, 4);
    ASSERT_EQ(res3, 2);
}

TEST(lua_script, multiple_function_calls)
{
    auto script = game::LuaScript{R"(
function get_num1(num)
        return num + 1
end
function get_num2(num)
        return num + 2
end
    )"};

    {
        script.set_function("get_num1");
        script.set_argument(static_cast<std::int64_t>(42l));
        script.execute(1u, 1u);
        auto res = std::int64_t{};
        script.get_result(res);

        ASSERT_EQ(res, 43);
    }

    {
        script.set_function("get_num2");
        script.set_argument(static_cast<std::int64_t>(43l));
        script.execute(1u, 1u);
        auto res = std::int64_t{};
        script.get_result(res);

        ASSERT_EQ(res, 45);
    }
}

TEST(lua_script, multiple_nested_function_calls)
{
    auto script = game::LuaScript{R"(
function get_num1(num)
        return get_num2(num) + 1
end
function get_num2(num)
        return num + 2
end
    )"};

    {
        script.set_function("get_num1");
        script.set_argument(static_cast<std::int64_t>(42l));
        script.execute(1u, 1u);
        auto res = std::int64_t{};
        script.get_result(res);

        ASSERT_EQ(res, 45);
    }
}

TEST(lua_script, to_string_with_empty_stack)
{
    auto script = game::LuaScript{R"(
function print_v(i, f, s)
        print(i, f, s)
end)"};

    const auto str = std::format("{}", script);

    // ASSERT_EQ(str, "<empty stack>");
    ASSERT_EQ(str, "LUA_TTABLE\n");
}

TEST(lua_script, to_string_with_func)
{
    auto script = game::LuaScript{R"(
function print_v(i, f, s)
        print(i, f, s)
end)"};

    script.set_argument(static_cast<std::int64_t>(1));
    script.set_argument(3.f);
    script.set_argument("hello");
    script.set_function("print_v");

    const auto str = std::format("{}", script);

    const auto expected = R"(LUA_TFUNCTION
LUA_TSTRING 'hello'
LUA_NUMBER (float) 3
LUA_NUMBER (int) 1
LUA_TTABLE
)";

    ASSERT_EQ(str, expected);
}

TEST(lua_script, function_call_vector3_arg)
{
    auto script = game::LuaScript{R"(
function print_v(v)
        print(v.x, v.y, v.z)
end)"};

    script.set_function("print_v");
    script.set_argument(game::Vector3{1.0f, 2.0f, 3.0f});
    ::testing::internal::CaptureStdout();
    script.execute(1u, 0u);
    const auto output = ::testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "1.0\t2.0\t3.0\n");
}

TEST(lua_script, function_call_vector3_return)
{
    auto script = game::LuaScript{R"(
function get_v()
        local v = Vector3(1.0, 2.0, 3.0)
        return v
end)"};

    script.set_function("get_v");
    script.execute(0u, 1u);

    auto res = game::Vector3{};
    script.get_result(res);

    ASSERT_NEAR(res.x, 1.f, 0.001f);
    ASSERT_NEAR(res.y, 2.f, 0.001f);
    ASSERT_NEAR(res.z, 3.f, 0.001f);
}

TEST(lua_script, function_call_vector3_arg_and_return)
{

    auto script = game::LuaScript{R"(
function mutate_v(v)
        v.x = v.x + 10.0
        return v
end)"};

    script.set_function("mutate_v");
    script.set_argument(game::Vector3{1.0f, 2.0f, 3.0f});
    script.execute(1u, 1u);

    auto res = game::Vector3{};
    script.get_result(res);

    ASSERT_NEAR(res.x, 11.f, 0.001f);
    ASSERT_NEAR(res.y, 2.f, 0.001f);
    ASSERT_NEAR(res.z, 3.f, 0.001f);
}
