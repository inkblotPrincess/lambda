/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/base/error.hpp>

#include <string_view>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace lambda::os
{
    static auto ensure_os(bool Predicate, std::string_view Message) -> void
    {
        if (::DWORD LastError = ::GetLastError(); !Predicate && (LastError != 0))
            throw system_exception{std::error_code{static_cast<int>(LastError), std::system_category()}, Message};
    }
} // namespace lambda::os::win32