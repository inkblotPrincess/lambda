/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <cstddef>
#include <memory>
#include <string_view>

namespace lambda::io
{
    struct ostream
    {
        virtual ~ostream() = default;

        virtual auto write(std::string_view Output) -> std::size_t = 0;
        virtual auto flush() -> void;
    };
    
    [[nodiscard]] auto std_out() noexcept -> std::unique_ptr<ostream>;
    [[nodiscard]] auto std_err() noexcept -> std::unique_ptr<ostream>;
} // namespace lambda::io