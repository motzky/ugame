#include <gtest/gtest.h>

#include "math/vector3.h"
#include "scripting/lua_script.h"
#include "scripting/script_runner.h"

TEST(lua_interop, vector3_ctor_simple)
{
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

    ASSERT_EQ(output, "1.0\t2.0\t3.0\n");
}
