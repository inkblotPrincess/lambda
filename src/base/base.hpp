/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define LAMBDA_PLATFORM_WINDOWS 1
#else
    #error Unrecognised platform
#endif

#define LAMBDA_CONCAT_IMPL(A, B) A##B
#define LAMBDA_CONCAT_VAR(A, B)  LAMBDA_CONCAT_IMPL(A, B)

namespace lambda
{
    namespace log
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
    } // namespace log
    
    class exception : public std::runtime_error
    {
    public:
        template<class... arg_types>
        exception(std::format_string<arg_types...> Format, arg_types&&... Arguments)
            : runtime_error{std::format(Format, std::forward<arg_types>(Arguments)...)}
            , m_What{std::format("{}\n --> {}", std::runtime_error::what(), std::stacktrace::current(1))}
        {

        }

        auto what() const noexcept -> char const* override;

    private:
        std::string m_What;
    };

    class system_exception : public exception
    {
    public:
        system_exception(std::error_code ErrorCode, std::string_view Message);

        [[nodiscard]] auto error_code() const noexcept -> std::error_code;

    private:
        std::error_code m_ErrorCode;
    };

    template<class... arg_types>
    auto expect(bool Predicate, std::format_string<arg_types...> Format, arg_types&&... Arguments) -> void
    {
        if (!Predicate)
        {
            log::fatal(Format, std::forward<arg_types>(Arguments)...);
            log::fatal(" --> {}", std::stacktrace::current(1));
            
            std::terminate();
        }
    }
} // namespace lambda