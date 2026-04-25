/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::runtime
{
    auto application_run([[maybe_unused]] command_line_arguments const& Arguments) -> void
    {
        auto SyncedStreams = io::synchronise(io::std_out(), io::std_err());
        log::add_sink(std::make_unique<io::ostream_sink>(std::move(SyncedStreams[0]), log::level::debug, log::level::info));
        log::add_sink(std::make_unique<io::ostream_sink>(std::move(SyncedStreams[1]), log::level::warn, log::level::fatal));
        log::register_thread("main");

        log::debug("Hello world");
        log::info("Hello world");
        log::warn("Hello world");
        log::error("Hello world");
        log::fatal("Hello world");

        auto Window = os::window{{.Height = 720u, .Width = 1080u, .Title = "Lambda :3", .StartMode = os::window_mode::windowed}};

        auto Running = true;
        Window.set_event_handler([&Running](os::window_event const& Event) noexcept {
            Event >> util::match {
                [&Running]([[maybe_unused]] os::window_quit_event const& QuitEvent) noexcept
                {
                    Running = false;
                    log::info("Window quit event posted");
                }
            };

            // keep processing events if no quit has been requested
            return Running;
        });

        while (Running)
        {
            Window.process_events();
        }
    }
} // namespace lambda::runtime