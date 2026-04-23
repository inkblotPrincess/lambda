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
    template<class func_type, class return_type, class... arg_types>
    concept function_type = 
            std::invocable<std::remove_cvref_t<func_type>, arg_types...> && 
            std::convertible_to<std::invoke_result_t<std::remove_cvref_t<func_type>, arg_types...>, return_type>;
} // namespace lambda::util