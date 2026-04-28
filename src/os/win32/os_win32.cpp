/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB                0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB                   0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB      0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB            0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x00000002
#define WGL_CONTEXT_FLAGS_ARB                       0x2094

#define WGL_DRAW_TO_WINDOW_ARB                      0x2001
#define WGL_ACCELERATION_ARB                        0x2003
#define WGL_SUPPORT_OPENGL_ARB                      0x2010
#define WGL_DOUBLE_BUFFER_ARB                       0x2011
#define WGL_PIXEL_TYPE_ARB                          0x2013
#define WGL_COLOR_BITS_ARB                          0x2014
#define WGL_DEPTH_BITS_ARB                          0x2022
#define WGL_STENCIL_BITS_ARB                        0x2023

#define WGL_FULL_ACCELERATION_ARB                   0x2027
#define WGL_TYPE_RGBA_ARB                           0x202B

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

                ensure_os(::RegisterClassExA(&WindowClass), "Failed to register win32 window class");
            });
        }

        using wglChoosePixelFormatARB_func = BOOL(WINAPI*)(HDC, int const*, FLOAT const*, UINT, int*, UINT*);
        wglChoosePixelFormatARB_func wglChoosePixelFormatARB = nullptr;

        using wglCreateContextAttribsARB_func = HGLRC(WINAPI*)(HDC, HGLRC, int const*);
        wglCreateContextAttribsARB_func wglCreateContextAttribsARB = nullptr;

        auto load_attribute_functions() -> void
        {
            std::once_flag Flag;

            std::call_once(Flag, [&]() {
                auto const DummyWC = ::WNDCLASSA{.lpfnWndProc = DefWindowProcA, .hInstance = GetModuleHandleA(nullptr), .lpszClassName = "lambda_dummy"};
                ensure_os(::RegisterClassA(&DummyWC), "Failed to register dummy class");
                LAMBDA_DEFER([&DummyWC]() { ::UnregisterClassA(DummyWC.lpszClassName, DummyWC.hInstance); });

                auto const DummyHandle = ::CreateWindowExA(0, DummyWC.lpszClassName, "", 0, 0, 0, 0, 0, 0, 0, DummyWC.hInstance, 0);
                ensure_os(DummyHandle, "Failed to create dummy window");
                LAMBDA_DEFER([&DummyHandle]() { ::DestroyWindow(DummyHandle); });

                auto const DummyDC = ::GetDC(DummyHandle);
                LAMBDA_DEFER([&DummyHandle, &DummyDC]() { ::ReleaseDC(DummyHandle, DummyDC); });

                auto const FormatDesc = ::PIXELFORMATDESCRIPTOR{
                    .nSize         = sizeof(::PIXELFORMATDESCRIPTOR),
                    .nVersion      = 1,
                    .dwFlags       = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
                    .iPixelType    = PFD_TYPE_RGBA,
                    .cColorBits    = 32,
                    .cAlphaBits    = 8,
                    .cDepthBits    = 24,
                    .cStencilBits  = 8,
                    .iLayerType    = PFD_MAIN_PLANE,
                };

                auto const Format = ::ChoosePixelFormat(DummyDC, &FormatDesc);
                ensure_os(Format != 0, "Failed to choose pixel format");
                ensure_os(::SetPixelFormat(DummyDC, Format, &FormatDesc), "Failed to set pixel format");

                auto RenderingContext = ::wglCreateContext(DummyDC);
                ensure_os(RenderingContext, "Failed to create rendering context");
                LAMBDA_DEFER([&DummyDC, &RenderingContext]() { ::wglMakeCurrent(DummyDC, nullptr); ::wglDeleteContext(RenderingContext); });

                ensure_os(::wglMakeCurrent(DummyDC, RenderingContext), "Failed to set rendering context");

                wglChoosePixelFormatARB = reinterpret_cast<wglChoosePixelFormatARB_func>(wglGetProcAddress("wglChoosePixelFormatARB"));
                ensure_os(wglChoosePixelFormatARB != nullptr, "Failed to load wglChoosePixelFormatARB");

                wglCreateContextAttribsARB = reinterpret_cast<wglCreateContextAttribsARB_func>(wglGetProcAddress("wglCreateContextAttribsARB"));
                ensure_os(wglCreateContextAttribsARB != nullptr, "Failed to load wglCreateContextAttribsARB");
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

    struct opengl_context::state
    {
        ::HWND WindowHandle = nullptr;
        ::HDC DeviceContext = nullptr;
        ::HGLRC RenderingContext = nullptr;

        ~state()
        {
            if (WindowHandle)
            {
                ::wglMakeCurrent(DeviceContext, nullptr);
                ::wglDeleteContext(RenderingContext);

                ::ReleaseDC(WindowHandle, DeviceContext);
            }
        }
    };

    opengl_context::opengl_context(os::window& Window)
        : m_State{std::make_unique<opengl_context::state>()}
    {
        m_State->WindowHandle  = static_cast<::HWND>(Window.raw_handle());
        m_State->DeviceContext = ::GetDC(m_State->WindowHandle);

        detail::load_attribute_functions();

        int const FormatDescAttributes[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     32,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
            0 // this needs to be null-terminated
        };

        auto Format = int{};
        auto FormatCount = ::UINT{};
        ensure_os(detail::wglChoosePixelFormatARB(m_State->DeviceContext, FormatDescAttributes, 0, 1, &Format, &FormatCount), "Failed to choose format");
        ensure_os(FormatCount != 0, "Failed to get format count");

        auto FormatDesc = ::PIXELFORMATDESCRIPTOR{};
        ensure_os(::DescribePixelFormat(m_State->DeviceContext, Format, sizeof(FormatDesc), &FormatDesc), "Failed to describe format");
        ensure_os(::SetPixelFormat(m_State->DeviceContext, Format, &FormatDesc), "Failed to set format");

        int const Win32ContextAttributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,  4,
            WGL_CONTEXT_MINOR_VERSION_ARB,  6,
            WGL_CONTEXT_FLAGS_ARB,          WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB,   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0 // this needs to be null-terminated
        };

        m_State->RenderingContext = detail::wglCreateContextAttribsARB(m_State->DeviceContext, 0, Win32ContextAttributes);
        ensure_os(m_State->RenderingContext, "Failed to create rendering context");
        ensure_os(::wglMakeCurrent(m_State->DeviceContext, m_State->RenderingContext), "Failed to set current context");
    }

    auto opengl_context::load_function(char const* FunctionName) -> void*
    {
        auto* Func = ::wglGetProcAddress(FunctionName);
        ensure_os(Func != nullptr, "Failed to load function");

        return reinterpret_cast<void*>(Func);
    }

    auto opengl_context::swap_buffers() -> void
    {
        ensure_os(::SwapBuffers(m_State->DeviceContext), "Failed to swap buffers");
    }
} // namespace lambda::os::win32