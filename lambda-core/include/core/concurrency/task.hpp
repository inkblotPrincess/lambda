/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/base/logging.hpp>
#include <core/concurrency/awaitable_manager.hpp>
#include <core/memory/arena.hpp>

#include <coroutine>

namespace lambda::con
{
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