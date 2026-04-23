/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#define LAMBDA_DEFER(Function) \
    auto LAMBDA_CONCAT_VAR(Defer, __COUNTER__) = ::lambda::util::deferred_function{Function}

namespace lambda::util
{
    template<class func_type>
    requires function_type<func_type, void, void> && std::is_nothrow_invocable_v<func_type>
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
} // namespace lambda::util