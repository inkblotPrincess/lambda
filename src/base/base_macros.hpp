/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define LAMBDA_PLATFORM_WINDOWS 1
#else
    #error Unrecognised platform
#endif

#define LAMBDA_CONCAT_IMPL(A, B) A##B
#define LAMBDA_CONCAT_VAR(A, B)  LAMBDA_CONCAT_IMPL(A, B)