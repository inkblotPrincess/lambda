/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/concurrency/thread_pool.hpp>
#include <core/memory/arena.hpp>

#include <chrono>
#include <coroutine>

namespace lambda::con
{
    struct task;

    class awaitable_manager
    {
    public:
        explicit awaitable_manager(thread_pool& Pool, memory::arena& CoroutineArena);

        awaitable_manager(awaitable_manager const& Other) = delete;
        auto operator=(awaitable_manager const& Other) -> awaitable_manager& = delete;

        awaitable_manager(awaitable_manager&& Other) noexcept = delete;
        auto operator=(awaitable_manager&& Other) noexcept -> awaitable_manager& = delete;

        auto operator co_await()
        {
            struct awaitable
            {
                auto await_ready() const noexcept -> bool { return false; }
                auto await_resume() const noexcept -> void { }

                auto await_suspend(std::coroutine_handle<> Handle) -> void
                {
                    auto Lock = std::scoped_lock{Self.m_Mutex};
                    Self.m_NextTickQueue.push(Handle);
                }

                awaitable_manager& Self;
            };
            return awaitable{*this};
        }

        auto operator()(std::chrono::nanoseconds WaitTime)
        {
            struct awaitable
            {
                auto await_ready() const noexcept -> bool { return WaitTime <= std::chrono::nanoseconds::zero(); }
                auto await_resume() const noexcept -> void { }

                auto await_suspend(std::coroutine_handle<> Handle) -> void
                {
                    auto Lock = std::scoped_lock{Self.m_Mutex};
                    Self.m_TimerQueue.push(timer_awaitable{
                        .TimePoint = std::chrono::steady_clock::now() + WaitTime,
                        .Handle = Handle,
                    });
                }

                awaitable_manager& Self;
                std::chrono::nanoseconds WaitTime;
            };

            return awaitable{*this, WaitTime};
        }

        auto pump() -> void;

    private:
        // NOTE: allows task to directly access the arena for `operator new()`. alternative was exposing the arena
        //       directly which is unnecessary amounts of exposure. (since every coroutine would also have access)
        //       access to the awaitable_manager and thus the arena.) doing it this way minimises the risk of doing
        //       something stupid, since the coroutine arena should only be used by the task type anyway.
        friend struct task;

        struct timer_awaitable
        {
            std::chrono::steady_clock::time_point TimePoint;
            std::coroutine_handle<> Handle;

            friend auto operator>(timer_awaitable const& Lhs, timer_awaitable const& Rhs) noexcept -> bool
            {
                return Lhs.TimePoint > Rhs.TimePoint;
            }
        };

        auto rethrow_one() -> void;

    private:
        thread_pool& m_Pool;
        memory::arena& m_CoroutineArena;

        std::mutex m_Mutex;
        std::queue<std::coroutine_handle<>> m_NextTickQueue;
        std::priority_queue<timer_awaitable, std::vector<timer_awaitable>, std::greater<timer_awaitable>> m_TimerQueue;

        std::mutex m_ExceptionMutex;
        std::queue<std::exception_ptr> m_ExceptionQueue;
    };
} // namespace lambda::con