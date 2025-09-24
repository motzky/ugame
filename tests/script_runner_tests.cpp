#include <gtest/gtest.h>

#include <cstdint>

#include "scripting/lua_script.h"
#include "scripting/script_runner.h"

TEST(script_runner, single_arg_no_return)
{
    auto script = game::LuaScript{R"(
function my_print(msg)
        print(msg)
end)"};

    const auto runner = game::ScriptRunner{script};

    ::testing::internal::CaptureStdout();
    runner.execute("my_print", "hello world");
    const auto output = ::testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "hello world\n");
}

TEST(script_runner, multiple_arg_no_return)
{
    auto script = game::LuaScript{R"(
function my_print(msg, num)
        print(msg .. num)
end)"};

    const auto runner = game::ScriptRunner{script};

    ::testing::internal::CaptureStdout();
    runner.execute("my_print", "hello world", static_cast<std::int64_t>(11));
    const auto output = ::testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "hello world11\n");
}
