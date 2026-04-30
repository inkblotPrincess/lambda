/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/os/window.hpp>

namespace lambda::os
{
    class opengl_context
    {
    public:
        opengl_context() = delete;
        opengl_context(os::window& Window);

        ~opengl_context();

        opengl_context(opengl_context const& Other) = delete;
        auto operator=(opengl_context const& Other) -> opengl_context& = delete;

        opengl_context(opengl_context&& Other) noexcept;
        auto operator=(opengl_context&& Other) noexcept -> opengl_context&;

        auto load_function(char const* FunctionName) -> void*;
        auto swap_buffers() -> void;

    private:
        struct state;

    private:
        std::unique_ptr<opengl_context::state> m_State;
    };
} // namespace lambda::os