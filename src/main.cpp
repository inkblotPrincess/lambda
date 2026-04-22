/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

// NOTE: system includes
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// NOTE: .hpp
#include "base/base_inc.hpp"
#include "io/io_inc.hpp"

// NOTE: .cpp
#include "base/base_inc.cpp"
#include "io/io_inc.cpp"

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> int
{
	using namespace lambda;
	
	auto SyncedStreams = io::synchronise(io::std_out(), io::std_err());
	log::add_sink(std::make_unique<io::ostream_sink>(std::move(SyncedStreams[0]), log::level::debug, log::level::info));
	log::add_sink(std::make_unique<io::ostream_sink>(std::move(SyncedStreams[1]), log::level::warn, log::level::fatal));
	log::register_thread("main");

	log::debug("Hello world");
	log::info("Hello world");
	log::warn("Hello world");
	log::error("Hello world");
	log::fatal("Hello world");

	return 0;
}