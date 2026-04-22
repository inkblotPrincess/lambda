/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda
{
    class exception : public std::runtime_error
    {
    public:
        template<class... arg_types>
        exception(std::format_string<arg_types...> Format, arg_types&&... Arguments)
            : runtime_error{std::format(Format, std::forward<arg_types>(Arguments)...)}
            , m_What{std::format("{}\n --> {}", std::runtime_error::what(), std::stacktrace::current(1))}
        {

        }

        auto what() const noexcept -> char const* override
        {
            return m_What.c_str();
        }

    private:
        std::string m_What;
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