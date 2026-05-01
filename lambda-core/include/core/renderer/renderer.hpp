/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/base/error.hpp>
#include <core/math/vector.hpp>
#include <core/memory/arena.hpp>
#include <core/os/window.hpp>

#include <cstddef>
#include <cstring>
#include <span>
#include <memory>

namespace lambda::render
{
    enum class command_type
    {
        clear
    };

    struct clear_command
    {
        math::vec4f RGBA;
    };

    class command_buffer
    {
    public:
        command_buffer() = delete;
        explicit command_buffer(std::span<std::byte> Buffer);

        ~command_buffer() = default;

        command_buffer(command_buffer const& Other) = delete;
        auto operator=(command_buffer const& Other) -> command_buffer& = delete;

        command_buffer(command_buffer&& Other) noexcept = default;
        auto operator=(command_buffer&& Other) noexcept -> command_buffer& = default;

        template<class command_data>
        requires std::is_trivially_destructible_v<command_data>
        auto push(command_type Type, command_data const& Data) -> void
        {
            auto const Size = sizeof(header) + sizeof(command_data);

            expect(m_Offset + Size < m_Buffer.size(), "Out of capacity for command buffer");

            auto* Header = reinterpret_cast<header*>(m_Buffer.data() + m_Offset);
            Header->Type = Type;
            Header->Size = sizeof(command_data);

            std::memcpy(Header + 1, &Data, sizeof(command_data));
            m_Offset += Size;
        }

        template<class func_type>
        requires std::invocable<func_type&, command_type, void const*>
        auto for_each(func_type&& Func) const -> void
        {
            auto Offset = 0zu;
            while (Offset < m_Offset)
            {
                auto const* Header = reinterpret_cast<header const*>(m_Buffer.data() + Offset);
                auto const* Data   = reinterpret_cast<void const*>(Header + 1);

                std::invoke(std::forward<func_type>(Func), Header->Type, Data);

                Offset += sizeof(header) + Header->Size;
            }
        }

        auto clear() noexcept -> void;

    private:
        struct header
        {
            command_type Type;
            std::uint32_t Size;
        };

    private:
        std::span<std::byte> m_Buffer;
        std::size_t m_Offset;
    };

    class command_list
    {
    public:
        command_list(command_buffer& Buffer);

        auto clear(float R, float G, float B, float A) -> void;
        auto clear(math::vec3f RGB, float A = 1.0f) -> void;
        auto clear(math::vec4f RGBA) -> void;

    private:
        command_buffer& m_Buffer;
    };

    class backend
    {
    public:
        virtual ~backend() = default;

        virtual auto begin_frame() -> void = 0;
        virtual auto end_frame(command_buffer const& Commands) -> void = 0;
    };

    enum class api
    {
        directx,
        opengl,
        vulkan,
    };

    class renderer
    {
    public:
        struct config
        {
            api Api;
            std::size_t CommandBufferSize;
        };

    public:
        renderer() = delete;
        renderer(renderer::config const& Config, os::window& Window, memory::arena& Arena);

        ~renderer() = default;

        renderer(renderer const& Other) = delete;
        auto operator=(renderer const& Other) -> renderer& = delete;

        renderer(renderer&& Other) noexcept = default;
        auto operator=(renderer&& Other) noexcept -> renderer& = default;

        template<class func_type>
        requires std::invocable<func_type&, command_list&>
        auto submit(func_type&& Func) -> void
        {
            auto Commands = command_list{m_CommandBuffer};
            std::invoke(std::forward<func_type>(Func), Commands);
        }

        auto begin_frame() -> void;
        auto end_frame() -> void;

    private:
        os::window& m_Window;
        std::unique_ptr<backend> m_Backend;

        command_buffer m_CommandBuffer;
    };
} // namespace lambda::render