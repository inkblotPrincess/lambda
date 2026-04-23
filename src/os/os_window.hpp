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

    class window
    {
    public:
        struct config
        {
            std::uint32_t Height;
            std::uint32_t Width;

            std::string_view Title;
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

        template<class handler_type>
        requires util::function_type<handler_type, bool, window_event const&>
        auto handle_events(handler_type&& Handler) noexcept -> void
        {
            process_events();

            for (auto Event = poll_event(); Event.has_value(); Event = poll_event())
            {
                if (!std::invoke(Handler, *Event))
                    break;
            }
        }

        [[nodiscard]] auto raw_handle() noexcept -> void*;

    private:
        [[nodiscard]] auto poll_event() noexcept -> std::optional<window_event>;
        auto process_events() noexcept -> void;

    private:
        std::unique_ptr<window::state> m_State;
    };
} // namespace lambda::os