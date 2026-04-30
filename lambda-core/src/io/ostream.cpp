/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <core/io/ostream.hpp>

#include <core/io/file_ostream.hpp>

namespace lambda::io
{
    auto ostream::flush() -> void
    {
        // NOTE: intentionally left blank; we have this here to keep header cleaner
    }

    auto std_out() noexcept -> std::unique_ptr<ostream>
    {
        return std::make_unique<file_ostream>(stdout);
    }

    auto std_err() noexcept -> std::unique_ptr<ostream>
    {
        return std::make_unique<file_ostream>(stderr);
    }
} // namespace lambda::io