/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::io
{
    auto ostream::flush() -> void
    {
        // NOTE: intentionally left blank; we have this here to keep header cleaner
    }

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

    auto std_out() noexcept -> std::unique_ptr<ostream>
    {
        return std::make_unique<file_ostream>(stdout);
    }

    auto std_err() noexcept -> std::unique_ptr<ostream>
    {
        return std::make_unique<file_ostream>(stderr);
    }

    ostream_sink::ostream_sink(std::unique_ptr<ostream> Stream, log::level MinLevel, log::level MaxLevel) noexcept
        : m_Stream{std::move(Stream)}
        , m_MinLevel{MinLevel}
        , m_MaxLevel{MaxLevel}
    {

    }

    auto ostream_sink::put(log::sink_payload const& Payload) -> void
    {
        if (Payload.Level < m_MinLevel || Payload.Level > m_MaxLevel)
            return;

        auto const Now = std::chrono::floor<std::chrono::seconds>(Payload.Time);
        auto const Out = std::format(
            "[{:%Y-%m-%d %H:%M:%S}] {:<{}} {:<7} >> {}\n", 
            Now, Payload.ThreadName, Payload.ThreadNamePadding, to_string(Payload.Level), Payload.Message
        );

        m_Stream->write(Out);
    }
    
    auto ostream_sink::flush() -> void
    {
        m_Stream->flush();
    }
} // namespace lambda::io