#include "scheduler/task.h"

#include <coroutine>
#include <ranges>
#include <stdexcept>
#include <utility>

namespace game
{
    Task::Task(std::coroutine_handle<promise_type> handle)
        : _handle(handle)
    {
    }

    Task::Task(Task &&other)
        : _handle(std::exchange(other._handle, nullptr))
    {
    }

    auto Task::operator=(Task &&other) -> Task &
    {
        std::ranges::swap(_handle, other._handle);
        return *this;
    }

    Task::~Task()
    {
        if (_handle)
        {
            _handle.destroy();
        }
    }

    auto Task::can_resume() const -> bool
    {
        return _handle && !_handle.done();
    }

    auto Task::has_handle(std::coroutine_handle<> handle) const -> bool
    {
        return _handle == handle;
    }

    auto Task::resume() -> void
    {
        _handle.resume();
        if (_handle.promise().ex_ptr)
        {
            std::rethrow_exception(_handle.promise().ex_ptr);
        }
    }

    auto Task::native_handle() const -> std::coroutine_handle<>
    {
        return _handle;
    }
}