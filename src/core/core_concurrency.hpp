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
    class thread
    {
    public:
        template<class func_type, class... arg_types>
        thread(std::string_view Name, func_type&& Func, arg_types&&... Arguments)
            : m_Name{Name}
            , m_Exception{nullptr}
            , m_Thread{
                [this]<class... a>(std::stop_token StopToken, func_type&& Func, a&&... Arguments)
                {
                    try
                    {
                        std::invoke(std::forward<func_type>(Func), StopToken, std::forward<a>(Arguments)...);
                    }
                    catch (...)
                    {
                        m_Exception = std::current_exception();
                    }
                },
                std::forward<func_type>(Func),
                std::forward<arg_types>(Arguments)...
            }
        {

        }

        auto request_stop() -> void;
        auto exception() -> std::exception_ptr;

    private:
        std::string m_Name;
        std::exception_ptr m_Exception;
        std::jthread m_Thread;
    };

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

    struct task
    {
        struct promise_type
        {
            static auto operator new(std::size_t Size, awaitable_manager& Awaitable, auto&...) -> void*
            {
                if (auto* Memory = Awaitable.m_CoroutineArena.allocate_bytes(Size, alignof(promise_type)); Memory != nullptr)
                    return Memory;

                throw std::bad_alloc{};
            }

            static auto operator delete([[maybe_unused]] void* Memory, [[maybe_unused]] std::size_t Size) noexcept -> void
            {
                // arena does not perform individual frees so this is a no-op
            }

            auto initial_suspend() -> std::suspend_never { return {}; }
            auto final_suspend() noexcept -> std::suspend_never { return {}; }
            auto return_void() -> void {}

            auto unhandled_exception() -> void
            {
                log::error("Unhandled task exception");
            }

            auto get_return_object()
            {
                return task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }
        };

        std::coroutine_handle<promise_type> Handle;
    };
} // namespace lambda::con