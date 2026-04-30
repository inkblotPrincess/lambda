/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <core/os/window.hpp>

#include <core/base/auto_release.hpp>
#include <internal/win32/win32.hpp>

#include <mutex>
#include <queue>

namespace lambda::os
{
    auto release_hwnd(::HWND& hwnd) noexcept -> void;
    using hwnd_type = auto_release<::HWND, release_hwnd>;

    struct window::state
    {
        hwnd_type Handle;
        std::queue<window_event> EventQueue;

        ::WINDOWPLACEMENT Placement = {.length = sizeof(::WINDOWPLACEMENT)};
    };

    namespace detail
    {
        auto CALLBACK win32_message_callback(::HWND Handle, ::UINT Message, ::WPARAM WParam, ::LPARAM LParam) -> ::LRESULT
        {
            window::state* State = nullptr;
            if (Message != WM_CREATE)
                State = reinterpret_cast<window::state*>(::GetWindowLongPtrA(Handle, GWLP_USERDATA));

            switch (Message)
            {
                case WM_CREATE:
                {
                    auto* Create = reinterpret_cast<::CREATESTRUCT*>(LParam);
                    State = reinterpret_cast<window::state*>(Create->lpCreateParams);
                    
                    SetWindowLongPtrA(Handle, GWLP_USERDATA, reinterpret_cast<::LONG_PTR>(State));
                } break;

                case WM_CLOSE:
                case WM_DESTROY:
                {
                    State->EventQueue.emplace(window_quit_event{});
                } break;
            }

            return ::DefWindowProcA(Handle, Message, WParam, LParam);
        }

        constexpr ::LPCSTR WindowClassName = "lambda_class";
        auto register_class(::HINSTANCE Instance) -> void
        {
            static std::once_flag Flag;

            std::call_once(Flag, [Instance]() {
                auto const WindowClass = ::WNDCLASSEXA{
                    .cbSize        = sizeof(::WNDCLASSEXA),
                    .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                    .lpfnWndProc   = win32_message_callback,
                    .hInstance     = Instance,
                    .hCursor       = ::LoadCursorA(nullptr, IDC_ARROW),
                    .hbrBackground = static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH)),
                    .lpszClassName = WindowClassName
                };

                ensure_os(::RegisterClassExA(&WindowClass), "Failed to register win32 window class");
            });
        }
    } // namespace detail

    auto release_hwnd(::HWND& hwnd) noexcept -> void
    {
        if (hwnd)
            ::DestroyWindow(hwnd);

        hwnd = nullptr;
    }

    window::window(window::config const& Config)
        : m_State{std::make_unique<window::state>()}
        , m_EventHandler{}
        , m_Mode{window_mode::windowed}
    {
        auto const Instance = ::GetModuleHandleA(nullptr);
        detail::register_class(Instance);

        auto ClientRect = ::RECT{
            .left   = 0,
            .top    = 0,
            .right  = static_cast<int>(Config.Width),
            .bottom = static_cast<int>(Config.Height)
        };

        auto const WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        ensure_os(::AdjustWindowRect(&ClientRect, WindowStyle, FALSE), "Failed to adjust win32 window rect");
        
        auto const WindowHeight = ClientRect.bottom - ClientRect.top;
        auto const WindowWidth  = ClientRect.right - ClientRect.left;

        auto const Handle = ::CreateWindowExA(
            WS_EX_APPWINDOW,
            detail::WindowClassName,
            Config.Title.data(),
            WindowStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            WindowWidth,
            WindowHeight,
            nullptr,
            nullptr,
            Instance,
            m_State.get()
        );
        ensure_os(Handle, "Failed to create win32 window");
        m_State->Handle.reset(Handle);

        set_mode(Config.StartMode);
    }

    window::~window() = default;

    window::window(window&& Other) noexcept = default;
    auto window::operator=(window&& Other) noexcept -> window& = default;

    auto window::poll_event() noexcept -> std::optional<window_event>
    {
        if (!std::ranges::empty(m_State->EventQueue))
        {
            auto const Event = m_State->EventQueue.front();
            m_State->EventQueue.pop();

            return Event;
        }

        return std::nullopt;
    }

    auto window::raw_handle() noexcept -> void*
    {
        return static_cast<void*>(*m_State->Handle);
    }

    auto window::set_mode(window_mode Mode) -> void
    {
        if (m_Mode == Mode)
            return;

        switch (Mode)
        {
            using enum window_mode;
            
            case fullscreen:
            {
                ensure_os(::GetWindowPlacement(*m_State->Handle, &m_State->Placement), "GetWindowPlacement failed");

                auto const Style = ::GetWindowLongPtrA(*m_State->Handle, GWL_STYLE);
                ensure_os(::SetWindowLongPtrA(*m_State->Handle, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW) != 0,"SetWindowLongPtrA failed");

                auto const Monitor = ::MonitorFromWindow(*m_State->Handle, MONITOR_DEFAULTTONEAREST);
                auto MonitorInfo = ::MONITORINFO{.cbSize = sizeof(::MONITORINFO)};
                ensure_os(::GetMonitorInfoA(Monitor, &MonitorInfo), "GetMonitorInfo failed");

                ensure_os(
                    ::SetWindowPos(
                        *m_State->Handle,
                        HWND_TOP,
                        MonitorInfo.rcMonitor.left,
                        MonitorInfo.rcMonitor.top,
                        MonitorInfo.rcMonitor.right  - MonitorInfo.rcMonitor.left,
                        MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                        SWP_NOOWNERZORDER | SWP_FRAMECHANGED
                    ),
                    "SetWindowPos failed"
                );
            } break;
            
            case windowed:
            {
                auto const Style = ::GetWindowLongPtrA(*m_State->Handle, GWL_STYLE);
                ensure_os(::SetWindowLongPtrA(*m_State->Handle, GWL_STYLE, Style | WS_OVERLAPPEDWINDOW) != 0, "SetWindowLongPtrA restore failed");
                ensure_os(::SetWindowPlacement(*m_State->Handle, &m_State->Placement), "SetWindowPlacement failed");

                ensure_os(
                    ::SetWindowPos(
                        *m_State->Handle,
                        nullptr,
                        0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE |
                        SWP_NOZORDER | SWP_NOOWNERZORDER |
                        SWP_FRAMECHANGED
                    ),
                    "SetWindowPos restore failed"
                );
            } break;
        }

        m_Mode = Mode;
    }

    auto window::update_event_queue() noexcept -> void
    {
        auto Message = ::MSG{};
        while (::PeekMessageA(&Message, *m_State->Handle, 0, 0, PM_REMOVE) != 0)
        {
            ::TranslateMessage(&Message);
            ::DispatchMessageA(&Message);
        }
    }
} // namespace lambda::os::win32