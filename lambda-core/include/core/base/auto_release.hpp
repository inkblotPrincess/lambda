/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda
{
    template<class handle_type, auto CloseFn, handle_type InvalidHandleValue = handle_type{}>
    class auto_release
    {
    public:
        constexpr auto_release() noexcept = default;

        explicit constexpr auto_release(handle_type Handle) noexcept
            : m_Handle{Handle}
        {

        }

        constexpr ~auto_release()
        {
            reset();
        }

        auto_release(auto_release const& Other) = delete;
        auto operator=(auto_release const& Other) -> auto_release& = delete;

        constexpr auto_release(auto_release&& Other) noexcept
            : m_Handle{Other.release()}
        {

        }

        constexpr auto operator=(auto_release&& Other) noexcept -> auto_release&
        {
            if (this != &Other)
                reset(Other.release());

            return *this;
        }

        [[nodiscard]] constexpr auto release() noexcept -> handle_type
        {
            handle_type Result = m_Handle;
            m_Handle = InvalidHandleValue;
            return Result;
        }

        constexpr auto reset(handle_type NewHandle = InvalidHandleValue) noexcept -> void
        {
            if (m_Handle != InvalidHandleValue)
                CloseFn(m_Handle);

            m_Handle = NewHandle;
        }

        [[nodiscard]] constexpr auto get() const noexcept -> handle_type { return m_Handle; }
        [[nodiscard]] constexpr auto operator*() const noexcept -> handle_type const& { return m_Handle; }
        [[nodiscard]] constexpr auto operator*() noexcept -> handle_type& { return m_Handle; }
        [[nodiscard]] constexpr explicit operator bool() const noexcept { return m_Handle != InvalidHandleValue; }

    private:
        handle_type m_Handle = InvalidHandleValue;
    };
} // namespace lambda