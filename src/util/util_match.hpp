/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

 #pragma once

namespace lambda::util
{
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
} // namespace lambda::util