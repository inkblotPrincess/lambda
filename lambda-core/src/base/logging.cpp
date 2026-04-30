/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <core/base/logging.hpp>

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace lambda::log
{
    namespace detail
    {
        std::atomic<std::size_t> MaxThreadNameLength = 3;

        std::mutex ThreadNamesMutex;
        std::unordered_map<std::thread::id, std::string> ThreadNames;

        std::mutex SinksMutex;
        std::vector<std::unique_ptr<log::sink>> Sinks;

        auto put_message(log::level Level, std::string_view Message, std::string_view ThreadName)
        {
            auto const Payload = log::sink_payload{
                .Time              = std::chrono::system_clock::now(),
                .Level             = Level,
                .Message           = Message,
                .ThreadName        = ThreadName,
                .ThreadNamePadding = MaxThreadNameLength.load()
            };

            auto Lock = std::lock_guard{SinksMutex};
            for (auto& Sink : Sinks)
                Sink->put(Payload);
        }
    } // namespace detail

    // c++26: replace with enum to string
    auto to_string(level Level) noexcept -> std::string_view
    {
        switch (Level)
        {
            using enum level;
            case debug: return "[debug]";
            case info:  return "[info]";
            case warn:  return "[warn]";
            case error: return "[error]";
            case fatal: return "[fatal]";
        }
        std::unreachable();
    }

    auto add_sink(std::unique_ptr<sink> Sink) noexcept -> void
    {
        auto const Lock = std::lock_guard{detail::SinksMutex};
        detail::Sinks.push_back(std::move(Sink));
    }

    auto register_thread(std::string ThreadName) -> void
    {
        auto const Id = std::this_thread::get_id();
        {
            auto Lock = std::lock_guard{detail::ThreadNamesMutex};

            if (auto It = detail::ThreadNames.find(Id); It != detail::ThreadNames.end())
                return;

            detail::ThreadNames[Id] = ThreadName;
        }

        auto CurrentMaxLength = detail::MaxThreadNameLength.load();
        while (CurrentMaxLength < ThreadName.size() &&  !detail::MaxThreadNameLength.compare_exchange_weak(CurrentMaxLength, ThreadName.size()));

        info("Thread '{}' registered (id: {})", ThreadName, Id);
    }
    
    auto unregister_thread(std::thread::id ThreadId) -> void
    {
        auto ThreadName = std::string{};
        {
            auto Lock = std::lock_guard{detail::ThreadNamesMutex};

            auto It = detail::ThreadNames.find(ThreadId);
            if (It == detail::ThreadNames.end())
                return;

            ThreadName = std::move(It->second);
            detail::ThreadNames.erase(It);
        }
        detail::put_message(level::info, std::format("Thread '{}' unregistered (id: {})", ThreadName, ThreadId), ThreadName);
    }

    auto put_message(level Level, std::string_view Message) -> void
    {
        auto const Id = std::this_thread::get_id();
        auto ThreadName = std::string{"???"};
        {
            auto Lock = std::lock_guard{detail::ThreadNamesMutex};

            if (auto It = detail::ThreadNames.find(Id); It != detail::ThreadNames.end())
                ThreadName = It->second;
        }

        detail::put_message(Level, Message, ThreadName);
    }
} // namespace lambda::log