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
#include <span>
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

// NOTE: .hpp
#include "core/core_base.hpp"
#include "core/core_memory.hpp"
#include "core/core_math.hpp"
#include "core/core_io.hpp"
#include "core/core_concurrency.hpp"

// NOTE: .cpp
#include "core/core_memory.cpp"
#include "core/core_base.cpp"
#include "core/core_io.cpp"
#include "core/core_concurrency.cpp"

#include <gtest/gtest.h>

// NOTE: tests
#include "core/tests/core_math_tests.cpp"

auto main(int argc, char** argv) -> int
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}