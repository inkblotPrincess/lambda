/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <variant>

namespace lambda::os
{
    class window_quit_event
    {
        // NOTE: intentionally left blank; quit event has no payload
    };

    using window_event = std::variant<window_quit_event>;
} // namespace lambda::os