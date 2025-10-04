#pragma once

#include <coroutine>

namespace game
{
    class Task
    {
    public:
        struct promise_type
        {
            auto initial_suspend()
            {
                return std::suspend_always{};
            }

            auto final_suspend() noexcept
            {
                return std::suspend_always{};
            }

            auto return_void()
            {
            }

            auto get_return_object()
            {
                return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            auto unhandled_exception() {}
        };

        Task(const Task &) = delete;
        auto operator=(const Task &other) -> Task & = delete;
        Task(Task &&other);
        auto operator=(Task &&other) -> Task &;
        ~Task();

        auto can_resume() const -> bool;

        auto has_handle(std::coroutine_handle<> handle) const -> bool;

        auto resume() -> void;

    private:
        explicit Task(std::coroutine_handle<promise_type>);

        std::coroutine_handle<promise_type> _handle;
    };

}