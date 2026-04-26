/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::con
{
    auto thread::request_stop() -> void
    {
        m_Thread.request_stop();
    }

    auto thread::exception() -> std::exception_ptr
    {
        return m_Exception;
    }

    thread_pool::thread_pool()
        : thread_pool(std::clamp(std::jthread::hardware_concurrency() - 1u, 1u, 32u))
    {

    }

    thread_pool::thread_pool(std::uint32_t WorkerCount)
        : m_Mutex{}
        , m_CV{}
        , m_Jobs{}
        , m_PendingJobs{0u}
        , m_Workers{}
    {
        log::info("Starting thread pool with {} workers", WorkerCount);

        m_Workers.reserve(WorkerCount);
        for (std::size_t I = 0u; I < WorkerCount; ++I)
        {
            auto const Name = std::format("tp_worker_{}", I);
            m_Workers.push_back({
                Name, 
                [this, Name](std::stop_token StopToken) 
                { 
                    worker(Name, std::move(StopToken)); 
                }
            });
        }
    }

    thread_pool::~thread_pool()
    {
        for (auto& Worker : m_Workers)
            Worker.request_stop();

        m_CV.notify_all();
    }

    auto thread_pool::add(job Job) -> void
    {
        {
            auto Lock = std::scoped_lock{m_Mutex};
            m_Jobs.push(std::move(Job));
            m_PendingJobs.fetch_add(1, std::memory_order_relaxed);
        }
        m_CV.notify_one();
    }

    auto thread_pool::drain() const -> void
    {
        auto Count = m_PendingJobs.load(std::memory_order_acquire);
        
        while (Count != 0u)
        {
            m_PendingJobs.wait(Count, std::memory_order_acquire);
            Count = m_PendingJobs.load(std::memory_order_acquire);
        }
    }

    auto thread_pool::worker(std::string WorkerName, std::stop_token StopToken) -> void
    {
        log::register_thread(WorkerName);
        
        while (true)
        {
            auto Job = job{};
            {
                auto Lock = std::unique_lock{m_Mutex};
                m_CV.wait(Lock, StopToken, [&]() { 
                    return !m_Jobs.empty(); 
                });

                if (StopToken.stop_requested() && m_Jobs.empty())
                    break;

                Job = std::move(m_Jobs.front());
                m_Jobs.pop();
            }
            
            Job();

            if (m_PendingJobs.fetch_sub(1, std::memory_order_acq_rel) == 1u)
                m_PendingJobs.notify_all();
        }

        log::unregister_thread(std::this_thread::get_id());
    }

    awaitable_manager::awaitable_manager(thread_pool& Pool)
        : m_Pool{Pool}
        , m_Mutex{}
        , m_NextTickQueue{}
        , m_TimerQueue{}
        , m_ExceptionMutex{}
        , m_ExceptionQueue{}
    {

    }

    auto awaitable_manager::pump() -> void
    {
        auto Ready = std::vector<std::coroutine_handle<>>{};
        {
            auto Lock = std::scoped_lock{m_Mutex};
            while (!m_NextTickQueue.empty())
            {
                Ready.push_back(m_NextTickQueue.front());
                m_NextTickQueue.pop();
            }

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