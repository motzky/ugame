#include <gtest/gtest.h>

#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"

#include "log.h"
#include "test_utils.h"

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

TEST(scheduler, simple_await)
{
    TEST_IMPL(
        auto sched = game::Scheduler{};

        sched.add([](game::Scheduler &scheduler) -> game::Task
                  { 
                    auto x = 0;
                    for(;;)
                    {
                        if(x == 5)
                        {
                            break;
                        }
                        game::log::debug("co_await");
                        co_await game::Wait{scheduler, 3u};
                        ++x;
                    } }(sched));

        sched.run();

    )
}