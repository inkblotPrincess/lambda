/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

// NOTE: system includes
#include <array>
#include <atomic>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <stacktrace>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "base/base.hpp"
#if defined(LAMBDA_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <GL/gl.h>

    #pragma comment(lib, "gdi32")
    #pragma comment(lib, "opengl32")
    #pragma comment(lib, "user32")
#endif

// NOTE: .hpp
#include "util/util.hpp"
#include "io/io.hpp"
#include "os/os.hpp"
#include "render/render.hpp"
#include "render/opengl/render_opengl.hpp"
#include "runtime/runtime.hpp"

#if defined(LAMBDA_PLATFORM_WINDOWS)
    #include "os/win32/os_win32.hpp"
#endif

// NOTE: .cpp
#include "base/base.cpp"
#include "util/util.cpp"
#include "io/io.cpp"
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
    using namespace lambda;

    try
    {
        auto Arguments = runtime::command_line_arguments{
            .Count     = static_cast<std::size_t>(argc + 1),
            .Arguments = argc > 1 ? argv + 1 : nullptr
        };

        runtime::application_run(Arguments);
    }
    catch (exception& Ex)
    {
        log::fatal("lambda::exception: {}", Ex.what());
        return EXIT_FAILURE;
    }
    catch (std::exception& Ex)
    {
        log::fatal("std::exception: {}", Ex.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        log::fatal("Unrecognised exception");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}