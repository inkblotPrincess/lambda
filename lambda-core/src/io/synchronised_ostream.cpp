/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <core/io/synchronised_ostream.hpp>

namespace lambda::io
{
    synchronised_ostream::synchronised_ostream(std::shared_ptr<ostream> Stream, std::vector<std::weak_ptr<ostream>> SyncedStreams) noexcept
        : m_Stream{std::move(Stream)}
        , m_SyncedStreams{std::move(SyncedStreams)}
    {

    }

    auto synchronised_ostream::write(std::string_view Output) -> std::size_t
    {
        for (auto& SyncedStream : m_SyncedStreams)
        {
            if (auto Stream = SyncedStream.lock())
                Stream->flush();
        }
        return m_Stream->write(Output);
    }

    auto synchronised_ostream::flush() -> void
    {
        m_Stream->flush();
    }
} // namespace lambda::io