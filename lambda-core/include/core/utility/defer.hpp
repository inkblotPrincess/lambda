/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/utility/macro_utils.hpp>

#include <concepts>
#include <type_traits>

#define LAMBDA_DEFER(...) \
    auto LAMBDA_CONCAT_VAR(Defer, __COUNTER__) = ::lambda::utility::deferred_function{__VA_ARGS__}

namespace lambda::utility
{
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
} // namespace lambda