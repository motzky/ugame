#include <gtest/gtest.h>

#include "core/exception.h"
#include "math/vector3.h"
#include "scripting/lua_script.h"
#include "scripting/script_runner.h"

#include "utils.h"

TEST(lua_interop, vector3_ctor_simple)
{
    TEST_IMPL(

        auto script = game::LuaScript{R"(
function print_vec()
        local v = Vector3(1.0, 2.0, 3.0)
        print(v.x, v.y, v.z)
end
        )"};

        const auto runner = game::ScriptRunner{script};

        ::testing::internal::CaptureStdout();
        runner.execute("print_vec");
        const auto output = ::testing::internal::GetCapturedStdout();

        ASSERT_EQ(output, "1.0\t2.0\t3.0\n");)
}

TEST(lua_interop, function_call_vector3_add)
{
    TEST_IMPL(
        auto script = game::LuaScript{R"(
function mutate_v(v1, v2)
        return v1 + v2
end)"};

        const auto runner = game::ScriptRunner{script};
        const auto res = runner.execute<game::Vector3>("mutate_v", game::Vector3{1.0f, 2.0f, 3.0f}, game::Vector3{1.0f, 2.0f, 3.0f});

        ASSERT_NEAR(res.x, 2.f, 0.001f);
        ASSERT_NEAR(res.y, 4.f, 0.001f);
        ASSERT_NEAR(res.z, 6.f, 0.001f);)
}

TEST(lua_interop, function_call_vector3_sub)
{
    TEST_IMPL(
        auto script = game::LuaScript{R"(
function mutate_v(v1, v2)
        return v1 - v2
end)"};

        const auto runner = game::ScriptRunner{script};
        const auto res = runner.execute<game::Vector3>("mutate_v", game::Vector3{1.0f, 2.0f, 3.0f}, game::Vector3{1.0f, 2.0f, 3.0f});

        ASSERT_NEAR(res.x, 0.f, 0.001f);
        ASSERT_NEAR(res.y, 0.f, 0.001f);
        ASSERT_NEAR(res.z, 0.f, 0.001f);)
}

TEST(lua_interop, function_call_vector3_mul)
{
    TEST_IMPL(
        auto script = game::LuaScript{R"(
function mutate_v(v1, f)
        return v1 * f
end)"};

        const auto runner = game::ScriptRunner{script};
        const auto res = runner.execute<game::Vector3>("mutate_v", 10.f, game::Vector3{1.0f, 2.0f, 3.0f});

        ASSERT_NEAR(res.x, 10.f, 0.001f);
        ASSERT_NEAR(res.y, 20.f, 0.001f);
        ASSERT_NEAR(res.z, 30.f, 0.001f);)
}

TEST(lua_interop, function_call_vector3_unm)
{
    TEST_IMPL(
        auto script = game::LuaScript{R"(
function mutate_v(v1)
        return -v1
end)"};

        const auto runner = game::ScriptRunner{script};
        const auto res = runner.execute<game::Vector3>("mutate_v", game::Vector3{1.0f, 2.0f, 3.0f});

        ASSERT_NEAR(res.x, -1.f, 0.001f);
        ASSERT_NEAR(res.y, -2.f, 0.001f);
        ASSERT_NEAR(res.z, -3.f, 0.001f);)
}

TEST(lua_interop, function_call_vector3_eq)
{
    TEST_IMPL(
        auto script = game::LuaScript{R"(
function is_vector_equal(v1, v2)
        return v1 == v2
end)"};

        const auto runner = game::ScriptRunner{script};
        const auto res = runner.execute<bool>("is_vector_equal", game::Vector3{1.0f, 2.0f, 3.0f}, game::Vector3{1.0f, 2.0f, 3.0f});

        ASSERT_TRUE(res);)
}

TEST(lua_interop, function_call_vector3_to_string)
{
    TEST_IMPL(
        auto script = game::LuaScript{R"(
function mutate_v(v1)
        return tostring(v1)
end)"};

        const auto runner = game::ScriptRunner{script};
        const auto res = runner.execute<std::string>("mutate_v", game::Vector3{1.0f, 2.0f, 3.0f});

        ASSERT_EQ(res, "x=1 y=2 z=3");)
}

TEST(lua_interop, function_call_vector3_distance)
{
    TEST_IMPL(
        auto script = game::LuaScript{R"(
function mutate_v(v1, v2)
        return distance(v1, v2)
end)"};

        const auto runner = game::ScriptRunner{script};
        const auto res = runner.execute<float>("mutate_v", game::Vector3{1.0f, 2.0f, 3.0f}, game::Vector3{1.0f, 2.0f, 3.0f});

        ASSERT_FLOAT_EQ(res, 0.f);)
}
