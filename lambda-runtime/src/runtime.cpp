/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <runtime/runtime.hpp>

#include <core/base/match.hpp>

#include <core/concurrency/awaitable_manager.hpp>
#include <core/concurrency/task.hpp>

#include <core/io/ostream.hpp>
#include <core/io/ostream_sink.hpp>
#include <core/io/synchronised_ostream.hpp>

#include <core/math/vector.hpp>

#include <core/memory/arena.hpp>

#include <core/os/window.hpp>
#include <core/os/window_events.hpp>

#include <core/renderer/renderer.hpp>

using namespace std::literals;

namespace lambda::runtime
{
    namespace detail
    {
        auto change_clear_colour(con::awaitable_manager& awaitable, math::vec3f& RGB) -> con::task
        {
            while (true)
            {
                co_await awaitable(1s);
                
                RGB += math::vec3f{0.05f, 0.2f, 0.6f};
                
                if (RGB.X > 1.0f) RGB.X -= 1.0f;
                if (RGB.Y > 1.0f) RGB.Y -= 1.0f;
                if (RGB.Z > 1.0f) RGB.Z -= 1.0f;
            }
        }
    } // namespace detail

    auto application_run(application_options const& Options) -> void
    {
        auto SyncedStreams = io::synchronise(io::std_out(), io::std_err());
        log::add_sink(std::make_unique<io::ostream_sink>(std::move(SyncedStreams[0]), log::level::debug, log::level::info));
        log::add_sink(std::make_unique<io::ostream_sink>(std::move(SyncedStreams[1]), log::level::warn, log::level::fatal));
        log::register_thread("main");

        auto Window = os::window{{.Height = 720u, .Width = 1080u, .Title = Options.WindowTitle, .StartMode = os::window_mode::windowed}};
        auto Renderer = render::renderer{{.Api = render::api::opengl, .BufferSize = 1024 * 1024}, Window};

        auto Running = true;
        Window.set_event_handler([&Running](os::window_event const& Event) noexcept {
            Event >> match {
                [&Running]([[maybe_unused]] os::window_quit_event const& QuitEvent) noexcept
                {
                    Running = false;
                    log::info("Window quit event posted");
                }
            };

            // keep processing events if no quit has been requested
            return Running;
        });

        auto PersistentArena = memory::arena{memory::mb_to_b(1zu)};
        auto FrameArena = memory::arena{memory::mb_to_b(1zu)};

        auto ThreadPool = con::thread_pool{};
        auto AwaitableManager = con::awaitable_manager{ThreadPool, PersistentArena};

        auto RGB = math::vec3f{0.0f};
        detail::change_clear_colour(AwaitableManager, RGB);

        while (Running)
        {
            FrameArena.reset();
            
            Window.process_events();
            if (!Running)
                break;

            AwaitableManager.pump();
            ThreadPool.drain();

            Renderer.begin_frame();
            Renderer.submit([=](render::command_list& Commands) {
                Commands.clear(RGB, 1.0f);
            });
            Renderer.end_frame();
        }
    }
} // namespace lambda::runtime