/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda::os
{
    class window_quit_event
    {
        // NOTE: intentionally left blank; quit event has no payload
    };

    using window_event = std::variant<window_quit_event>;

    enum class window_mode
    {
        fullscreen,
        windowed
    };

    class window
    {
    public:
        struct config
        {
            std::uint32_t Height;
            std::uint32_t Width;

            std::string_view Title;

            window_mode StartMode;
        };

        struct state;

    public:
        window() = delete;
        window(window::config const& Config);

        ~window();

        window(window const& Other) = delete;
        auto operator=(window const& Other) -> window& = delete;

        window(window&& Other) noexcept;
        auto operator=(window&& Other) noexcept -> window&;

        auto process_events() noexcept -> void;
        [[nodiscard]] auto raw_handle() noexcept -> void*;
        auto set_event_handler(std::function<bool(window_event const&)> EventHandler) noexcept -> void;
        auto set_mode(window_mode Mode) -> void;

    private:
        [[nodiscard]] auto poll_event() noexcept -> std::optional<window_event>;
        auto update_event_queue() noexcept -> void;

    private:
        std::unique_ptr<window::state> m_State;
        std::function<bool(window_event const&)> m_EventHandler;
        window_mode m_Mode;
    };

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