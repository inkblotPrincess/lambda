/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::os::win32
{
    auto release_hwnd(::HWND& hwnd) noexcept -> void
    {
        if (hwnd)
            ::DestroyWindow(hwnd);

        hwnd = nullptr;
    }
} // namespace lambda::os::win32