/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/concurrency/thread.hpp>

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

namespace lambda::con
{
    using job = std::move_only_function<void()>;

    class thread_pool
    {
    public:
        thread_pool();
        thread_pool(std::uint32_t WorkerCount);

        ~thread_pool();

        auto add(job Job) -> void;
        auto drain() const -> void;

    private:
        auto worker(std::string WorkerName, std::stop_token StopToken) -> void;

    private:
        std::mutex m_Mutex;
        std::condition_variable_any m_CV;

        std::queue<job> m_Jobs;
        std::atomic_uint32_t m_PendingJobs;

        std::vector<thread> m_Workers;
    };
} // namespace lambda::con