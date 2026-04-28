/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::os
{
    window::~window() = default;

    window::window(window&& Other) noexcept = default;
    auto window::operator=(window&& Other) noexcept -> window& = default;

    auto window::process_events() noexcept -> void
    {
        update_event_queue();
        if (!m_EventHandler)
            return;

        for (auto Event = poll_event(); Event.has_value(); Event = poll_event())
        {
            if (!std::invoke(m_EventHandler, *Event))
                break;
        }
    }

    auto window::set_event_handler(std::function<bool(window_event const&)> EventHandler) noexcept -> void
    {
        m_EventHandler = EventHandler;
    }

    opengl_context::~opengl_context() = default;

    opengl_context::opengl_context(opengl_context&& Other) noexcept = default;
    auto opengl_context::operator=(opengl_context&& Other) noexcept -> opengl_context& = default;
} // namespace lambda::os