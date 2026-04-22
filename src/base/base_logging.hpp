/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda::log
{
    enum class level
    {
        debug,
        info,
        warn,
        error,
        fatal
    };

    // c++26: replace with enum to string
    auto to_string(level Level) noexcept -> std::string_view;

    struct sink_payload
    {
        std::chrono::system_clock::time_point Time;

        level Level;
        
        std::string_view Message;
        std::string_view ThreadName;

        std::size_t ThreadNamePadding;
    };

    struct sink
    {
        virtual ~sink() = default;

        virtual auto put(sink_payload const& Payload) -> void = 0;
        virtual auto flush() -> void = 0;
    };

    auto add_sink(std::unique_ptr<sink> Sink) noexcept -> void;

    auto register_thread(std::string ThreadName) -> void;
    auto unregister_thread(std::thread::id ThreadId) -> void;

    auto put_message(level Level, std::string_view Message);

    template<class... argument_types>
    static auto debug(std::format_string<argument_types...> Format, argument_types&&... Arguments) -> void
    {
        put_message(level::debug, std::format(Format, std::forward<argument_types>(Arguments)...));
    }

    template<class... argument_types>
    static auto info(std::format_string<argument_types...> Format, argument_types&&... Arguments) -> void
    {
        put_message(level::info, std::format(Format, std::forward<argument_types>(Arguments)...));
    }

    template<class... argument_types>
    static auto warn(std::format_string<argument_types...> Format, argument_types&&... Arguments) -> void
    {
        put_message(level::warn, std::format(Format, std::forward<argument_types>(Arguments)...));
    }

    template<class... argument_types>
    static auto error(std::format_string<argument_types...> Format, argument_types&&... Arguments) -> void
    {
        put_message(level::error, std::format(Format, std::forward<argument_types>(Arguments)...));
    }

    template<class... argument_types>
    static auto fatal(std::format_string<argument_types...> Format, argument_types&&... Arguments) -> void
    {
        put_message(level::fatal, std::format(Format, std::forward<argument_types>(Arguments)...));
    }
} // namespace lambda::log