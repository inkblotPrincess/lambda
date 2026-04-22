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
		// TODO: assert that m_File is valid(?)
		return std::fwrite(Output.data(), 1, Output.size(), m_File);
	}

	auto file_ostream::flush() -> void
	{
		// TODO: assert that m_File is valid(?)
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
} // namespace lambda::io