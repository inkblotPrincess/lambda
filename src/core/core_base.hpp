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

#if defined(__clang__)
    #define LAMBDA_COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #define LAMBDA_COMPILER_MSVC 1
#elif defined(__GNUC__)
    #define LAMBDA_COMPILER_GCC 1
#else
    #error "Unsupported compiler"
#endif

#define LAMBDA_CONCAT_IMPL(A, B) A##B
#define LAMBDA_CONCAT_VAR(A, B)  LAMBDA_CONCAT_IMPL(A, B)

#define LAMBDA_DEFER(...) \
    auto LAMBDA_CONCAT_VAR(Defer, __COUNTER__) = ::lambda::deferred_function{__VA_ARGS__}

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

    template<class... handler_types>
    struct match : handler_types...
    {
        using handler_types::operator()...;
    };

    template<class... handler_types>
    match(handler_types...) -> match<handler_types...>;

    template<class variant_type, class matcher_type>
    constexpr auto operator>>(variant_type&& Variant, matcher_type&& Matcher) -> decltype(auto)
    {
        return std::visit(std::forward<matcher_type>(Matcher), std::forward<variant_type>(Variant));
    }

    template<class func_type>
    requires std::invocable<func_type&> && std::same_as<std::invoke_result_t<func_type&>, void>
    class [[nodiscard]] deferred_function
    {
    public:
        deferred_function() = delete;
        constexpr explicit deferred_function(func_type&& Function) noexcept(std::is_nothrow_move_constructible_v<func_type>)
            : m_Function{std::forward<func_type>(Function)}
        {

        }

        constexpr ~deferred_function()
        {
            std::invoke(m_Function);
        }

        deferred_function(deferred_function const& Other) = delete;
        auto operator=(deferred_function const& Other) -> deferred_function& = delete;

        deferred_function(deferred_function&& Other) noexcept = delete;
        auto operator=(deferred_function&& Other) noexcept -> deferred_function& = delete;

    private:
        func_type m_Function;
    };

    template<class func_type>
    deferred_function(func_type) -> deferred_function<std::decay_t<func_type>>;

    template<class handle_type, auto CloseFn, handle_type InvalidHandleValue = handle_type{}>
    class auto_release
    {
    public:
        constexpr auto_release() noexcept = default;

        explicit constexpr auto_release(handle_type Handle) noexcept
            : m_Handle{Handle}
        {

        }

        constexpr ~auto_release()
        {
            reset();
        }

        auto_release(auto_release const& Other) = delete;
        auto operator=(auto_release const& Other) -> auto_release& = delete;

        constexpr auto_release(auto_release&& Other) noexcept
            : m_Handle{Other.release()}
        {

        }

        constexpr auto operator=(auto_release&& Other) noexcept -> auto_release&
        {
            if (this != &Other)
                reset(Other.release());

            return *this;
        }

        [[nodiscard]] constexpr auto release() noexcept -> handle_type
        {
            handle_type Result = m_Handle;
            m_Handle = InvalidHandleValue;
            return Result;
        }

        constexpr auto reset(handle_type NewHandle = InvalidHandleValue) noexcept -> void
        {
            if (m_Handle != InvalidHandleValue)
                CloseFn(m_Handle);

            m_Handle = NewHandle;
        }

        [[nodiscard]] constexpr auto get() const noexcept -> handle_type { return m_Handle; }
        [[nodiscard]] constexpr auto operator*() const noexcept -> handle_type const& { return m_Handle; }
        [[nodiscard]] constexpr auto operator*() noexcept -> handle_type& { return m_Handle; }
        [[nodiscard]] constexpr explicit operator bool() const noexcept { return m_Handle != InvalidHandleValue; }

    private:
        handle_type m_Handle = InvalidHandleValue;
    };
} // namespace lambda