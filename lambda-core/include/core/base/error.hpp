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

#include <format>
#include <stacktrace>
#include <stdexcept>
#include <system_error>

namespace lambda
{
    class exception : public std::runtime_error
    {
    public:
        template<class... arg_types>
        constexpr exception(std::format_string<arg_types...> Format, arg_types&&... Arguments)
            : runtime_error{std::format(Format, std::forward<arg_types>(Arguments)...)}
            , m_What{std::format("{}\n --> {}", std::runtime_error::what(), std::stacktrace::current(1))}
        {

        }

        constexpr auto what() const noexcept -> char const* override
        {
            return m_What.c_str();
        }

    private:
        std::string m_What;
    };

    class system_exception : public exception
    {
    public:
        constexpr system_exception(std::error_code ErrorCode, std::string_view Message)
            : exception{"System error ({}): {}", ErrorCode.message(), Message}
            , m_ErrorCode{ErrorCode}
        {

        }

        [[nodiscard]] constexpr auto error_code() const noexcept -> std::error_code
        {
            return m_ErrorCode;
        }

    private:
        std::error_code m_ErrorCode;
    };

    template<class... arg_types>
    constexpr auto expect(bool Predicate, std::format_string<arg_types...> Format, arg_types&&... Arguments) -> void
    {
        if (!Predicate)
        {
            log::fatal(Format, std::forward<arg_types>(Arguments)...);
            log::fatal(" --> {}", std::stacktrace::current(1));
            
            std::terminate();
        }
    }
} // namespace lambda