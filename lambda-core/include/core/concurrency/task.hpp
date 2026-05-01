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

#include <coroutine>

namespace lambda::con
{
    struct task
    {
        struct promise_type
        {
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