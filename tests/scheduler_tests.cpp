#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <vector>

#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"

#include "log.h"
#include "test_utils.h"

using namespace std::chrono_literals;

auto foo() -> game::Task
{
    co_return;
}

TEST(scheduler, simple)
{
    TEST_IMPL(
        auto done = false;
        auto sched = game::Scheduler{};

        sched.add([&done]() -> game::Task
                  {
                    done=true;
                    co_return; }());

        // ASSERT_FALSE(done);

        sched.run();

        ASSERT_TRUE(done);

    )
}

TEST(scheduler, simple_await_tick)
{
    // TEST_IMPL(
    auto log = std::vector<std::string>{};
    auto sched = game::Scheduler{};

    sched.add([](game::Scheduler &scheduler, std::vector<std::string> &log) -> game::Task
              { 
                    auto x = 0;
                    for(;;)
                    {
                        if(x == 5)
                        {
                            break;
                        }
                        log.push_back("co_await");
                        co_await game::Wait{scheduler, x+1u};
                        ++x;
                    } }(sched, log));

    sched.run();

    const auto expected = std::vector<std::string>{
        "co_await",
        "co_await",
        "co_await",
        "co_await",
        "co_await",
    };

    ASSERT_EQ(log, expected);
    // )
}
TEST(scheduler, simple_await_ticks_two_tasks)
{
    // TEST_IMPL(
    auto log = std::vector<std::string>{};
    auto sched = game::Scheduler{};

    sched.add([](game::Scheduler &scheduler, std::vector<std::string> &log) -> game::Task
              { 
                    auto x = 0;
                    for(;;)
                    {
                        if(x == 5)
                        {
                            break;
                        }
                        log.push_back("co_await 1");
                        co_await game::Wait{scheduler, 1u};
                        ++x;
                    } }(sched, log));

    sched.add([](game::Scheduler &scheduler, std::vector<std::string> &log) -> game::Task
              { 
                    auto x = 0;
                    for(;;)
                    {
                        if(x == 5)
                        {
                            break;
                        }
                        log.push_back("co_await 2");
                        co_await game::Wait{scheduler, 2u};
                        ++x;
                    } }(sched, log));

    sched.run();

    const auto expected = std::vector<std::string>{
        "co_await 2",
        "co_await 1",
        "co_await 1",
        "co_await 2",
        "co_await 1",
        "co_await 1",
        "co_await 2",
        "co_await 1",
        "co_await 2",
        "co_await 2",
    };

    ASSERT_EQ(log, expected);

    // )
}

TEST(scheduler, simple_await_time)
{
    // TEST_IMPL(
    auto log = std::vector<std::string>{};
    auto sched = game::Scheduler{};

    sched.add([](game::Scheduler &scheduler, std::vector<std::string> &log) -> game::Task
              {
                  log.push_back("startiong");
                  co_await game::Wait{scheduler, 50ms}; }(sched, log));

    sched.run();

        // )
}
