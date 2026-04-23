/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda::os::win32
{
    [[noreturn]] auto throw_last_error(std::string_view Message, ::DWORD LastError = ::GetLastError())
    {
        throw system_exception{std::error_code{static_cast<int>(LastError), std::system_category()}, Message};
    }

    auto release_hwnd(::HWND& hwnd) noexcept -> void;
    using hwnd_type = util::auto_release<::HWND, release_hwnd>;
} // namespace lambda::os::win32