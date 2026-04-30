/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <thread>

namespace lambda::con
{
    class thread
    {
    public:
        template<class func_type, class... arg_types>
        thread(std::string_view Name, func_type&& Func, arg_types&&... Arguments)
            : m_Name{Name}
            , m_Exception{nullptr}
            , m_Thread{
                [this]<class... a>(std::stop_token StopToken, func_type&& Func, a&&... Arguments)
                {
                    try
                    {
                        std::invoke(std::forward<func_type>(Func), StopToken, std::forward<a>(Arguments)...);
                    }
                    catch (...)
                    {
                        m_Exception = std::current_exception();
                    }
                },
                std::forward<func_type>(Func),
                std::forward<arg_types>(Arguments)...
            }
        {

        }

        auto request_stop() noexcept -> void
        {
            m_Thread.request_stop();
        }

        auto exception() const noexcept -> std::exception_ptr
        {
            return m_Exception;
        }

    private:
        std::string m_Name;
        std::exception_ptr m_Exception;
        std::jthread m_Thread;
    };
} // namespace lambda::con