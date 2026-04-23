/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::os
{
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

                if (!::RegisterClassExA(&WindowClass))
                    throw_last_error("Failed to register win32 window class");
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
        if (!::AdjustWindowRect(&ClientRect, WindowStyle, FALSE))
            throw_last_error("Failed to adjust win32 window rect");
        
        auto const WindowHeight = ClientRect.bottom - ClientRect.top;
        auto const WindowWidth  = ClientRect.right - ClientRect.left;

        auto const Handle = ::CreateWindowExA(
            0,
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

        if (!Handle)
            throw_last_error("Failed to create win32 window");
        
        m_State->Handle.reset(Handle);
        m_State->Instance = Instance;
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