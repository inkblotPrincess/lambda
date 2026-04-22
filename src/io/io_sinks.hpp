/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda::io
{
	class ostream_sink : public log::sink
	{
	public:
		ostream_sink(std::unique_ptr<ostream> Stream, log::level MinLevel, log::level MaxLevel) noexcept;

		~ostream_sink() override = default;

		ostream_sink(ostream_sink const& Other) = delete;
		auto operator=(ostream_sink const& Other) -> ostream_sink& = delete;

		ostream_sink(ostream_sink&& Other) noexcept = default;
		auto operator=(ostream_sink&& Other) noexcept -> ostream_sink& = default;

		auto put(log::sink_payload const& Payload) -> void override;
		auto flush() -> void override;

	private:
		std::unique_ptr<ostream> m_Stream;
		log::level m_MinLevel;
		log::level m_MaxLevel;
	};
} // namespace lambda::io