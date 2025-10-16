#include <gtest/gtest.h>

#include <chrono>
#include <print>
#include <string>
#include <vector>

#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"

#include "log.h"
#include "test_utils.h"

using namespace std::chrono_literals;

auto await_foo() -> game::Task
{
    std::println("foo");
    co_return;
}

TEST(scheduler, simple)
{
    TEST_IMPL(
        auto done = false;
        auto bus = game::messaging::MessageBus{};
        auto sched = game::Scheduler{bus};

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
    auto bus = game::messaging::MessageBus{};
    auto sched = game::Scheduler{bus};

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
    auto bus = game::messaging::MessageBus{};
    auto sched = game::Scheduler{bus};

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
        "co_await 1",
        "co_await 2",
        "co_await 1",
        "co_await 1",
        "co_await 2",
        "co_await 1",
        "co_await 1",
        "co_await 2",
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
    auto bus = game::messaging::MessageBus{};
    auto sched = game::Scheduler{bus};

    sched.add([](game::Scheduler &scheduler, std::vector<std::string> &log) -> game::Task
              {
                  log.push_back("starting");
                  co_await game::Wait{scheduler, 50ms};
                  log.push_back("done"); }(sched, log));

    const auto start = std::chrono::steady_clock::now();
    sched.run();
    const auto end = std::chrono::steady_clock::now();

    const auto expected = std::vector<std::string>{
        "starting",
        "done",
    };

    ASSERT_EQ(log, expected);
    ASSERT_GE(end - start, 50ms);

    // )
}

TEST(scheduler, simple_await_task)
{
    // TEST_IMPL(
    auto log = std::vector<std::string>{};
    auto bus = game::messaging::MessageBus{};
    auto sched = game::Scheduler{bus};

    sched.add([](game::Scheduler &scheduler, [[maybe_unused]] std::vector<std::string> &log) -> game::Task
              {
                  std::println("starting");
                  co_await game::Wait{scheduler, await_foo()};
                  std::println("done"); }(sched, log));

    ::testing::internal::CaptureStdout();

    sched.run();

    const auto output = ::testing::internal::GetCapturedStdout();
    const auto expected = std::string{"starting\nfoo\ndone\n"};

    ASSERT_EQ(output, expected);

    // )
}

TEST(scheduler, exception)
{
    // TEST_IMPL(
    auto log = std::vector<std::string>{};
    auto bus = game::messaging::MessageBus{};
    auto sched = game::Scheduler{bus};

    sched.add([] -> game::Task
              {
                throw game::Exception("bad coro");
                co_return; }());

    ASSERT_THROW(sched.run(), game::Exception);

    // )
}
