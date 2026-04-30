/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/io/ostream.hpp>

#include <cstdio>

namespace lambda::io
{
    class file_ostream : public ostream
    {
    public:
        file_ostream() = delete;
        file_ostream(FILE* File) noexcept;

        ~file_ostream() override;

        file_ostream(file_ostream const& Other) = delete;
        auto operator=(file_ostream const& Other) -> file_ostream& = delete;

        file_ostream(file_ostream&& Other) noexcept;
        auto operator=(file_ostream&& Other) noexcept -> file_ostream&;

        auto write(std::string_view Output) -> std::size_t override;
        auto flush() -> void override;

    private:
        FILE* m_File;
    };
} // namespace lambda::io