/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <core/concurrency/awaitable_manager.hpp>
#include <core/concurrency/task.hpp>

namespace lambda::con
{
    awaitable_manager::awaitable_manager(thread_pool& Pool, memory::arena& CoroutineArena)
        : m_Pool{Pool}
        , m_CoroutineArena{CoroutineArena}
        , m_Mutex{}
        , m_NextTickQueue{}
        , m_TimerQueue{}
        , m_ExceptionMutex{}
        , m_ExceptionQueue{}
    {

    }

    auto awaitable_manager::pump() -> void
    {
        // TODO: make this vector backed by frame arena
        auto Ready = std::vector<std::coroutine_handle<>>{};

        {
            // next-frame coroutines
            auto Lock = std::scoped_lock{m_Mutex};
            while (!m_NextTickQueue.empty())
            {
                Ready.push_back(m_NextTickQueue.front());
                m_NextTickQueue.pop();
            }

            // timer-based coroutines
            auto const Now = std::chrono::steady_clock::now();
            while (!m_TimerQueue.empty() && m_TimerQueue.top().TimePoint <= Now)
            {
                Ready.push_back(m_TimerQueue.top().Handle);
                m_TimerQueue.pop();
            }
        }

        for (auto Handle : Ready)
        {
            m_Pool.add([this, Handle]() {
                try
                {
                    Handle.resume();
                }
                catch (...)
                {
                    auto Lock = std::scoped_lock{m_ExceptionMutex};
                    m_ExceptionQueue.push(std::current_exception());
                }
            });
        }

        rethrow_one();
    }

    auto awaitable_manager::rethrow_one() -> void
    {
        auto Exception = std::exception_ptr{};
        {
            auto Lock = std::scoped_lock{m_ExceptionMutex};
            if (m_ExceptionQueue.empty())
                return;

            Exception = std::move(m_ExceptionQueue.front());
            m_ExceptionQueue.pop();
        }
        std::rethrow_exception(Exception);
    }
} // namespace lambda::con