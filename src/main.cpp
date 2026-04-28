/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

// NOTE: system includes
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <stacktrace>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

using namespace std::literals;

// NOTE: platform includes
#include "core/core_base.hpp"
#if defined(LAMBDA_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <GL/gl.h>

    #if defined(LAMBDA_COMPILER_MSVC)
        #pragma comment(lib, "gdi32")
        #pragma comment(lib, "opengl32")
        #pragma comment(lib, "user32")
    #endif
#endif

// NOTE: .hpp
#include "core/core_math.hpp"
#include "core/core_io.hpp"
#include "core/core_concurrency.hpp"

#include "os/os.hpp"

#include "render/render.hpp"
#include "render/opengl/render_opengl.hpp"

#include "runtime/runtime.hpp"

#if defined(LAMBDA_PLATFORM_WINDOWS)
    #include "os/win32/os_win32.hpp"
#endif

// NOTE: .cpp
#include "core/core_base.cpp"
#include "core/core_io.cpp"
#include "core/core_concurrency.cpp"

#include "os/os.cpp"

#include "render/render.cpp"
#include "render/opengl/render_opengl.cpp"

#include "runtime/runtime.cpp"

#if defined(LAMBDA_PLATFORM_WINDOWS)
    #include "os/win32/os_win32.cpp"
    #include "render/opengl/render_opengl_win32.cpp"
#endif

auto main(int argc, char* argv[]) -> int
{
    try
    {
        auto Arguments = lambda::runtime::command_line_arguments{
            .Count     = static_cast<std::size_t>(argc + 1),
            .Arguments = argc > 1 ? argv + 1 : nullptr
        };

        lambda::runtime::application_run(Arguments);
    }
    catch (lambda::exception& Ex)
    {
        lambda::log::fatal("lambda::exception: {}", Ex.what());
        return EXIT_FAILURE;
    }
    catch (std::exception& Ex)
    {
        lambda::log::fatal("std::exception: {}", Ex.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        lambda::log::fatal("Unrecognised exception");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}