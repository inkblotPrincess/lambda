/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda::os
{
    auto ensure_os(bool Predicate, std::string_view Message) -> void
    {
        if (::DWORD LastError = ::GetLastError(); !Predicate && (LastError != 0))
            throw system_exception{std::error_code{static_cast<int>(LastError), std::system_category()}, Message};
    }

    auto release_hwnd(::HWND& hwnd) noexcept -> void;
    using hwnd_type = auto_release<::HWND, release_hwnd>;

    struct window::state
    {
        hwnd_type Handle;
        std::queue<window_event> EventQueue;

        ::WINDOWPLACEMENT Placement = {.length = sizeof(::WINDOWPLACEMENT)};
    };
} // namespace lambda::os::win32