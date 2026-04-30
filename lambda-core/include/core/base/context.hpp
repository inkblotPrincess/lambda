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

#if defined(__clang__)
    #define LAMBDA_COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #define LAMBDA_COMPILER_MSVC 1
#elif defined(__GNUC__)
    #define LAMBDA_COMPILER_GCC 1
#else
    #error "Unsupported compiler"
#endif

#define LAMBDA_CONCAT_IMPL(A, B) A##B
#define LAMBDA_CONCAT_VAR(A, B)  LAMBDA_CONCAT_IMPL(A, B)