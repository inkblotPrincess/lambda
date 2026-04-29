/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::memory
{
    arena::scope::scope(arena& Arena)
        : m_Arena{Arena}
        , m_UsedMark{Arena.m_Used}
        , m_DestructorChainMark{Arena.m_DestructorChainTail}
    {

    }

    arena::scope::~scope()
    {
        m_Arena.destroy_chain(m_DestructorChainMark);
        m_Arena.m_Used = m_UsedMark;
    }

    arena::arena(std::size_t Capacity)
        : m_Buffer{std::make_unique<std::byte[]>(Capacity)}
        , m_Capacity{Capacity}
        , m_Used{0zu}
        , m_DestructorChainTail{nullptr}
    {

    }

    arena::~arena()
    {
        if (m_DestructorChainTail)
            destroy_chain();
    }

    arena::arena(arena&& Other) noexcept
        : m_Buffer{std::move(Other.m_Buffer)}
        , m_Capacity{std::exchange(Other.m_Capacity, 0zu)}
        , m_Used{std::exchange(Other.m_Used, 0zu)}
        , m_DestructorChainTail{std::exchange(Other.m_DestructorChainTail, nullptr)}
    {

    }

    auto arena::operator=(arena&& Other) noexcept -> arena&
    {
        if (this != &Other)
        {
            if (m_DestructorChainTail)
                destroy_chain();

            m_Buffer = std::move(Other.m_Buffer);
            
            m_Capacity = std::exchange(Other.m_Capacity, 0zu);
            m_Used     = std::exchange(Other.m_Used, 0zu);
            
            m_DestructorChainTail = std::exchange(Other.m_DestructorChainTail, nullptr);

        }
        return *this;
    }

    auto arena::allocate_bytes(std::size_t Size, std::size_t Alignment) noexcept -> void*
    {
        expect(Alignment > 0, "expected non-zero alignment");
        expect((Alignment & (Alignment - 1)) == 0, "expected power of two alignment");

        auto Base    = reinterpret_cast<std::uintptr_t>(m_Buffer.get());
        auto Aligned = (Base + m_Used + (Alignment - 1)) & ~(Alignment - 1);
        auto NewUsed = (Aligned - Base) + Size;

        if (NewUsed > m_Capacity)
            return nullptr;

        m_Used = NewUsed;

        return reinterpret_cast<void*>(Aligned);
    }

    auto arena::capacity() const noexcept -> std::size_t
    {
        return m_Capacity;
    }

    auto arena::remaining() const noexcept -> std::size_t
    {
        return m_Capacity - m_Used;
    }

    auto arena::reset() noexcept -> void
    {
        destroy_chain();
        m_Used = 0;
    }

    auto arena::used() const noexcept -> std::size_t
    {
        return m_Used;
    }

    auto arena::destroy_chain(destructor_node* StoppingPoint) noexcept -> void
    {
        while (m_DestructorChainTail != StoppingPoint)
        {
            auto* Current = m_DestructorChainTail;
            m_DestructorChainTail = Current->Previous;

            Current->DestructorFn(Current->ObjectToDestroy);
        }
    }
} // namespace lambda::memory