/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <core/io/file_ostream.hpp>

namespace lambda::io
{
    file_ostream::file_ostream(FILE* File) noexcept
        : m_File{File}
    {

    }

    file_ostream::~file_ostream()
    {
        if (m_File)
            std::fclose(m_File);
    }

    file_ostream::file_ostream(file_ostream&& Other) noexcept
        : m_File{std::exchange(Other.m_File, nullptr)}
    {

    }

    auto file_ostream::operator=(file_ostream&& Other) noexcept -> file_ostream&
    {
        if (this != &Other)
        {
            if (m_File)
                std::fclose(m_File);

            m_File = std::exchange(Other.m_File, nullptr);
        }
        return *this;
    }

    auto file_ostream::write(std::string_view Output) -> std::size_t
    {
        if (!m_File)
            return 0;

        return std::fwrite(Output.data(), 1, Output.size(), m_File);
    }

    auto file_ostream::flush() -> void
    {
        if (!m_File)
            return;

        std::fflush(m_File);
    }
} // namespace lambda::io