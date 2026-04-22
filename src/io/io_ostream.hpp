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
	struct ostream
	{
		virtual ~ostream() = default;

		virtual auto write(std::string_view Output) -> std::size_t = 0;
		virtual auto flush() -> void;
	};

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

	class synchronised_ostream : public ostream
	{
	public:
		synchronised_ostream() = delete;
		synchronised_ostream(std::shared_ptr<ostream> Stream, std::vector<std::weak_ptr<ostream>> SyncedStreams) noexcept;

		~synchronised_ostream() override = default;

		synchronised_ostream(synchronised_ostream const& Other) = delete;
		auto operator=(synchronised_ostream const& Other) -> synchronised_ostream& = delete;

		synchronised_ostream(synchronised_ostream&& Other) noexcept = default;
		auto operator=(synchronised_ostream&& Other) noexcept -> synchronised_ostream& = default;

		auto write(std::string_view Output) -> std::size_t override;
		auto flush() -> void override;

	private:
		std::shared_ptr<ostream> m_Stream;
		std::vector<std::weak_ptr<ostream>> m_SyncedStreams;
	};

	[[nodiscard]] auto std_out() noexcept -> std::unique_ptr<ostream>;
	[[nodiscard]] auto std_err() noexcept -> std::unique_ptr<ostream>;

	template<class... stream_types>
	requires (std::same_as<std::remove_cvref_t<stream_types>, std::unique_ptr<ostream>> && ...)
	[[nodiscard]] auto synchronise(stream_types&&... Streams) noexcept -> std::array<std::unique_ptr<ostream>, sizeof...(Streams)>
	{
		auto Shared = std::array<std::shared_ptr<ostream>, sizeof...(Streams)>{std::shared_ptr<ostream>{std::move(Streams)}...};
		auto Result = std::array<std::unique_ptr<ostream>, sizeof...(Streams)>{};
		
		for (std::size_t I = 0u; I < Shared.size(); ++I)
		{
			std::vector<std::weak_ptr<ostream>> SyncedStreams;
			SyncedStreams.reserve(Shared.size() - 1);

			for (std::size_t J = 0u; J < Shared.size(); ++J)
			{
				if (I != J)
					SyncedStreams.emplace_back(Shared[J]);
			}

			Result[I] = std::make_unique<synchronised_ostream>(Shared[I], std::move(SyncedStreams));
		}

		return Result;
	}
} // namespace lambda::io