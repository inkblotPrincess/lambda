/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#define LAMBDA_DEFER(...) \
    auto LAMBDA_CONCAT_VAR(Defer, __COUNTER__) = ::lambda::util::deferred_function{__VA_ARGS__}

namespace lambda::util
{
    template<class func_type, class return_type, class... arg_types>
    concept function_type = 
            std::invocable<std::remove_cvref_t<func_type>, arg_types...> && 
            std::convertible_to<std::invoke_result_t<std::remove_cvref_t<func_type>, arg_types...>, return_type>;

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
        explicit deferred_function(func_type&& Function) noexcept(std::is_nothrow_move_constructible_v<func_type>)
            : m_Function{std::forward<func_type>(Function)}
        {

        }

        ~deferred_function()
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

        ~auto_release()
        {
            reset();
        }

        constexpr auto_release(auto_release const& Other) = delete;
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
} // namespace lambda::util