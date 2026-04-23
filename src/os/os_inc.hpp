/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include "os_window.hpp"

#if defined(LAMBDA_PLATFORM_WINDOWS)
    #define WIN32_MEAN_AND_LEAN
    #include <windows.h>
    
    #include "win32/os_win32_core.hpp"
    #include "win32/os_win32_window.hpp"
#endif