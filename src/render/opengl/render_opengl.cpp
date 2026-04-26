/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::render::opengl
{
    context::~context() = default;

    context::context(context&& Other) noexcept = default;
    auto context::operator=(context&& Other) noexcept -> context& = default;

    opengl_backend::opengl_backend(os::window& Window)
        : m_Context{Window}
        , m_Gl{[this](char const* FunctionName) { return m_Context.load_function(FunctionName); }}
        , m_DummyVAO{0u}
    {
        auto& gl = m_Gl;
        gl.GenVertexArrays(1, &m_DummyVAO);
    }

    auto opengl_backend::begin_frame() -> void
    {

    }

    auto opengl_backend::end_frame(command_buffer const& Commands) -> void
    {
        Commands.for_each([](command_type Type, void const* Data) {
            switch (Type)
            {
                using enum command_type;
                
                case clear:
                {
                    auto const& Command = *static_cast<clear_command const*>(Data);

                    ::glClearColor(Command.R, Command.G, Command.B, Command.A);
                    ::glClear(GL_COLOR_BUFFER_BIT);
                } break;
            }
        });
        
        m_Context.swap_buffers();
    }
} // namespace lambda::render::opengl