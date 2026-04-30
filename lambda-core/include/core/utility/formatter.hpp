/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/base/logging.hpp>

#include <concepts>
#include <format>
#include <meta>
#include <ranges>
#include <string>
#include <utility>

namespace lambda::utility
{
    template<class type> concept has_to_string_member = requires(type Type) {{ Type.to_string() } -> std::convertible_to<std::string>;};
    template<class type> concept has_to_string_free   = requires(type Type) {{ to_string(Type) } -> std::convertible_to<std::string>;};

    struct to_string_cpo
    {
        template<has_to_string_member type>
        constexpr auto operator()(type&& Object) const -> std::string
        {
            return Object.to_string();
        }

        template<class type>
        requires (!has_to_string_member<type> && has_to_string_free<type>)
        constexpr auto operator()(type&& Object) const -> std::string
        {
            return to_string(Object);
        }

        template<class type>
        requires (std::is_enum_v<type> && !has_to_string_free<type>)
        constexpr auto operator()(type&& Object) const -> std::string
        {
            template for (constexpr auto Element : std::define_static_array(std::meta::enumerators_of(^^std::remove_cvref_t<type>)))
            {
                if (Object == [:Element:])
                    return std::string{std::meta::identifier_of(Element)};
            }

            log::warn("Failed to find value for enum");
            return "<Unknown>";
        }
    };

    inline constexpr auto ToStringCPO = to_string_cpo{};

    template<class type>
    struct formatter : std::formatter<std::string>
    {
        auto format(const type& Object, std::format_context& Context) const
        {
            return std::formatter<std::string>::format(ToStringCPO(Object), Context);
        }
    };
} // namespace lambda

template<class type>
concept can_format = requires(type Type) 
{
    { lambda::utility::ToStringCPO(Type) } -> std::convertible_to<std::string>; 
};

template<can_format type>
struct std::formatter<type> : lambda::utility::formatter<type>
{

};