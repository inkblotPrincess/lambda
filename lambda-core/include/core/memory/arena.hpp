/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <span>
#include <type_traits>

namespace lambda::memory
{
    constexpr auto kb_to_b(std::size_t Kilobytes) noexcept -> std::size_t { return Kilobytes * 1024zu; }
    constexpr auto mb_to_b(std::size_t Megabytes) noexcept -> std::size_t { return Megabytes * 1024zu * 1024zu; }
    constexpr auto gb_to_b(std::size_t Gigabytes) noexcept -> std::size_t { return Gigabytes * 1024zu * 1024zu * 1024zu; }

    class arena
    {
    private:
        struct destructor_node
        {
            using destructor_fn = void(*)(void* Object) noexcept;

            destructor_fn DestructorFn = nullptr;
            void* ObjectToDestroy = nullptr;
            destructor_node* Previous = nullptr;
        };

    public:
        class scope
        {
        public:
            scope() = delete;
            explicit scope(arena& Arena) noexcept;

            ~scope();

            scope(scope const& Other) = delete;
            auto operator=(scope const& Other) -> scope& = delete;

            scope(scope&& Other) noexcept = delete;
            auto operator=(scope&& Other) noexcept -> scope& = delete;

        private:
            friend class arena;

        private:
            arena& m_Arena;
            std::size_t m_UsedMark;
            destructor_node* m_DestructorChainMark;
        };

    public:
        arena() = delete;
        explicit arena(std::size_t Capacity) noexcept;

        ~arena();

        arena(arena const& Other) = delete;
        auto operator=(arena const& Other) -> arena& = delete;

        arena(arena&& Other) noexcept;
        auto operator=(arena&& Other) noexcept -> arena&;

        template<class alloc_type>
        requires std::is_trivially_destructible_v<alloc_type> && std::is_default_constructible_v<alloc_type>
        [[nodiscard]] auto allocate(std::size_t Count) noexcept(std::is_nothrow_default_constructible_v<alloc_type>) -> std::span<alloc_type>
        {
            auto const UsedMark = m_Used;

            auto* const Memory = static_cast<alloc_type*>(allocate_bytes(Count * sizeof(alloc_type), alignof(alloc_type)));
            if (!Memory)
                return {};

            try
            {
                for (std::size_t I = 0zu; I < Count; ++I)
                    std::construct_at(Memory + I);

                return {Memory, Count};
            }
            catch(...)
            {
                // NOTE: have to unwind changes before re-throwing exception
                m_Used = UsedMark;

                // NOTE: we have to put this extra if-constexpr here in order to satisfy constraints related
                //       to noexcept; even though this line can only ever theoretically throw in cases where
                //       alloc_type throws on construction, the compiler still thinks we're trying to throw
                //       from a noexcept function in cases where alloc_type is nothrow constructible and gives
                //       a warning
                if constexpr (!std::is_nothrow_default_constructible_v<alloc_type>)
                    throw;
            }
        }

        template<class alloc_type, class... arg_types>
        requires std::constructible_from<alloc_type, arg_types...>
        [[nodiscard]] auto emplace(arg_types&&... Arguments) noexcept(std::is_nothrow_constructible_v<alloc_type, arg_types...>) -> alloc_type*
        {
            alloc_type* Object = nullptr;
            auto const UsedMark = m_Used;

            if constexpr (std::is_trivially_destructible_v<alloc_type>)
            {
                Object = static_cast<alloc_type*>(allocate_bytes(sizeof(alloc_type), alignof(alloc_type)));
                if (!Object)
                    return nullptr;
            }
            else
            {
                constexpr auto Alignment    = alignof(alloc_type) > alignof(destructor_node) ? alignof(alloc_type) : alignof(destructor_node);
                constexpr auto OffsetToType = (sizeof(destructor_node) + (alignof(alloc_type) - 1)) & ~(alignof(alloc_type) - 1);
                constexpr auto Size         = OffsetToType + sizeof(alloc_type);

                auto* const Memory = static_cast<std::byte*>(allocate_bytes(Size, Alignment));
                if (!Memory)
                    return nullptr;

                Object = reinterpret_cast<alloc_type*>(Memory + OffsetToType);

                auto* const Node = reinterpret_cast<destructor_node*>(Memory);
                Node->DestructorFn    = [](void* Object) noexcept { std::destroy_at(static_cast<alloc_type*>(Object)); };
                Node->ObjectToDestroy = Object;
                Node->Previous        = m_DestructorChainTail;

                m_DestructorChainTail = Node;
            }

            try
            {
                return std::construct_at(Object, std::forward<arg_types>(Arguments)...);
            }
            catch(...)
            {
                // NOTE: have to unwind changes before re-throwing exception
                m_Used = UsedMark;
                if constexpr (!std::is_trivially_destructible_v<alloc_type>)
                    m_DestructorChainTail = m_DestructorChainTail->Previous;

                // NOTE: we have to put this extra if-constexpr here in order to satisfy constraints related
                //       to noexcept; even though this line can only ever theoretically throw in cases where
                //       alloc_type throws on construction, the compiler still thinks we're trying to throw
                //       from a noexcept function in cases where alloc_type is nothrow constructible and gives
                //       a warning
                if constexpr (!std::is_nothrow_constructible_v<alloc_type, arg_types...>)
                    throw;
            }

            std::unreachable();
        }

        [[nodiscard]] auto allocate_bytes(std::size_t Size, std::size_t Alignment) noexcept -> void*;

        [[nodiscard]] auto capacity() const noexcept -> std::size_t;

        [[nodiscard]] auto remaining() const noexcept -> std::size_t;

        auto reset() noexcept -> void;

        [[nodiscard]] auto used() const noexcept -> std::size_t;

    private:
        auto destroy_chain(destructor_node* StoppingPoint = nullptr) noexcept -> void;

    private:
        std::unique_ptr<std::byte[]> m_Buffer;

        std::size_t m_Capacity;
        std::size_t m_Used;

        destructor_node* m_DestructorChainTail;
    };
} // namespace lambda::memory