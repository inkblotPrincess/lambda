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