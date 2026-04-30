/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/os/opengl_context.hpp>
#include <core/renderer/renderer.hpp>
#include <internal/opengl/opengl.hpp>

namespace lambda::render::opengl
{
    class opengl_backend : public backend
    {
    public:
        opengl_backend() = delete;
        opengl_backend(os::window& Window);

        ~opengl_backend() override = default;

        opengl_backend(opengl_backend const& Other) = delete;
        auto operator=(opengl_backend const& Other) -> opengl_backend& = delete;

        opengl_backend(opengl_backend&& Other) noexcept = default;
        auto operator=(opengl_backend&& Other) noexcept -> opengl_backend& = default;

        auto begin_frame() -> void override;
        auto end_frame(command_buffer const& Commands) -> void override;

    private:
        os::opengl_context m_Context;
        vtable m_Gl;
        ::GLuint m_DummyVAO;
    };
} // namespace lambda::render::opengl