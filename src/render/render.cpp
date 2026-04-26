/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::render
{
    namespace detail
    {
        [[nodiscard]] auto make_api(api Api, os::window& Window) -> std::unique_ptr<backend>
        {
            switch (Api)
            {
                using enum api;
                
                case directx: expect(false, "No backend support for DirectX"); 
                case opengl:  return std::make_unique<opengl::opengl_backend>(Window);
                case vulkan:  expect(false, "No backend support for Vulkan");
            }
            std::unreachable();
        }
    } // namespace detail

    renderer::renderer(api Api, os::window& Window)
        : m_Window{Window}
        , m_Backend{detail::make_api(Api, m_Window)}
    {

    }

    auto renderer::begin_frame() -> void
    {
        m_Backend->begin_frame();
    }

    auto renderer::end_frame() -> void
    {
        m_Backend->end_frame();
    }
} // namespace lambda::render