/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <core/renderer/renderer.hpp>
#include <internal/opengl/opengl_backend.hpp>

#include <utility>

namespace lambda::render
{
    namespace detail
    {
        [[nodiscard]] auto make_api(api Api, os::window& Window) -> std::unique_ptr<backend>
        {
            switch (Api)
            {
                using enum api;
                
                case directx: expect(false, "No backend support for DirectX"); break;
                case opengl:  return std::make_unique<opengl::opengl_backend>(Window);
                case vulkan:  expect(false, "No backend support for Vulkan"); break;
            }
            std::unreachable();
        }
    } // namespace detail

    command_buffer::command_buffer(std::span<std::byte> Buffer)
        : m_Buffer{Buffer}
        , m_Offset{0zu}
    {

    }

    auto command_buffer::clear() noexcept -> void
    {
        m_Offset = 0zu;
    }

    command_list::command_list(command_buffer& Buffer)
        : m_Buffer{Buffer}
    {

    }

    auto command_list::clear(float R, float G, float B, float A) -> void
    {
        clear(math::vec4f{R, G, B, A});
    }

    auto command_list::clear(math::vec3f RGB, float A) -> void
    {
        clear(math::vec4f{RGB, A});
    }
 
    auto command_list::clear(math::vec4f RGBA) -> void
    {
        m_Buffer.push(command_type::clear, clear_command{RGBA});
    }

    renderer::renderer(renderer::config const& Config, os::window& Window, memory::arena& Arena)
        : m_Window{Window}
        , m_Backend{detail::make_api(Config.Api, m_Window)}
        , m_CommandBuffer{Arena.allocate<std::byte>(Config.CommandBufferSize)}
    {

    }

    auto renderer::begin_frame() -> void
    {
        m_Backend->begin_frame();
    }

    auto renderer::end_frame() -> void
    {
        m_Backend->end_frame(m_CommandBuffer);
    }
} // namespace lambda::render