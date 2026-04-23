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

// NOTE: .hpp
#include "base/base.hpp"
#include "util/util_inc.hpp"
#include "io/io_inc.hpp"
#include "os/os_inc.hpp"
#include "runtime/runtime_inc.hpp"

// NOTE: .cpp
#include "base/base.cpp"
#include "util/util_inc.cpp"
#include "io/io_inc.cpp"
#include "os/os_inc.cpp"
#include "runtime/runtime_inc.cpp"

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