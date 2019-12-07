#include "win_app.h"

#include "win_common.h"
#include "win_ime.h"
#include "win_error.h"
#include "win_util.h"

#include "win_renderer_direct2d.h"

#include "common.h"
#include "string_utils.h"

namespace asr::win
{

namespace
{

void init_opengl()
{
    auto const instance = reinterpret_cast<HINSTANCE>(&__ImageBase);

    LUID id {};
    if (!::AllocateLocallyUniqueId(&id))
    {
        ASR_THROW(std::errc::interrupted, "main", "AllocateLocallyUniqueId failed");
    }

    auto const string_id = widen(to_hex(id));

    static std::exception_ptr e;

    auto const window_proc = [](
          HWND   const hWnd
        , UINT   const message
        , WPARAM const wParam
        , LPARAM const lParam) noexcept -> LRESULT
    {
        try
        {
            if (message != WM_CREATE && message != WM_NCCREATE)
            {
                return ::DefWindowProc(hWnd, message, wParam, lParam);
            }

            auto const dc = ::GetDC(hWnd);
            if (!dc)
            {
                ASR_THROW(std::errc::interrupted, "main", "GetDC failed");
            }

            PIXELFORMATDESCRIPTOR format {};
            format.nSize        = sizeof(format);
            format.nVersion     = 1;
            format.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            format.iPixelType   = PFD_TYPE_RGBA;
            format.cColorBits   = 32;
            format.cDepthBits   = 24;
            format.cAlphaBits   = 8;
            format.cStencilBits = 8;
            format.iLayerType   = PFD_MAIN_PLANE;

            auto const id = ::ChoosePixelFormat(dc, &format);
            if (!id)
            {
                ASR_THROW(std::errc::interrupted, "main", "ChoosePixelFormat failed");
            }

            if (!::SetPixelFormat(dc, id, &format))
            {
                ASR_THROW(std::errc::interrupted, "main", "SetPixelFormat failed");
            }

            auto const context = ::wglCreateContext(dc);
            if (!context)
            {
                ASR_THROW(std::errc::interrupted, "main", "wglCreateContext failed");
            }

            if (!::wglMakeCurrent(dc, context))
            {
                ASR_THROW(std::errc::interrupted, "main", "wglMakeCurrent failed");
            }

            return message == WM_CREATE
                ? ::DefWindowProc(hWnd, message, wParam, lParam)
                :  FALSE;
        }
        catch (...)
        {
            e = std::current_exception();
            return ::DefWindowProc(hWnd, message, wParam, lParam);
        }
    };

    WNDCLASSEXW wc {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = window_proc;
    wc.hInstance     = instance;
    wc.lpszClassName = string_id.data();

    auto const atom = ::RegisterClassExW(&wc);
    if (!atom)
    {
        ASR_THROW(std::errc::interrupted, "main", "RegisterClassExW failed");
    }

    auto const window = ::CreateWindowExW(
          0
        , wc.lpszClassName
        , wc.lpszClassName
        , WS_OVERLAPPED | WS_VISIBLE
        , CW_USEDEFAULT
        , CW_USEDEFAULT
        , CW_USEDEFAULT
        , CW_USEDEFAULT
        , nullptr
        , nullptr
        , instance
        , nullptr);

    ASR_ASSERT(!window);

    if (e)
    {
        ASR_THROW(e, "main", "");
    }
}

void register_for_input()
{
    RAWINPUTDEVICE dev[2] {};

    // HID mouse
    dev[0].usUsagePage = USHORT {0x01};
    dev[0].usUsage     = USHORT {0x02};
    dev[0].dwFlags     = 0; //RIDEV_NOLEGACY;
    dev[0].hwndTarget  = nullptr;

    // HID keyboard
    dev[1].usUsagePage = USHORT {0x01};
    dev[1].usUsage     = USHORT {0x06};
    dev[1].dwFlags     = 0; //RIDEV_NOLEGACY;
    dev[1].hwndTarget  = nullptr;

    if (!::RegisterRawInputDevices(dev, static_cast<UINT>(std::size(dev)), sizeof(RAWINPUTDEVICE)))
    {
        ASR_THROW(get_last_error(), "win", "RegisterRawInputDevices failed");
    }
}

} // namespace

application::application(application_options options)
    : m_input_manager(static_cast<text_input_manager*>(options.ime))
    , m_observer(options.observer)
{
    ASR_ASSERT(m_input_manager);
    ASR_ASSERT(m_observer);
}

void application::start()
{
    ASR_ASSERT(!m_initialized);

    auto const version = win_version();

    get_window_instance(this);

    auto const instance = reinterpret_cast<HINSTANCE>(&__ImageBase);

    LUID id {};
    if (!::AllocateLocallyUniqueId(&id))
    {
        ASR_THROW(get_last_error(), "win", "AllocateLocallyUniqueId failed");
    }

    auto const string_id = widen(to_hex(id));

    WNDCLASSEXW wc {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = top_level_window_proc;
    wc.hInstance     = instance;
    wc.lpszClassName = string_id.data();
    wc.hCursor       = reinterpret_cast<HCURSOR>(::LoadImageW(nullptr, MAKEINTRESOURCEW(32512), IMAGE_CURSOR, 0, 0, LR_SHARED | LR_DEFAULTSIZE));

    auto const atom = ::RegisterClassExW(&wc);
    if (!atom)
    {
        ASR_THROW(get_last_error(), "win", "RegisterClassExW failed");
    }

    auto const window = ::CreateWindowExW(
          version >= win8_1 ? 0 : 0
        , wc.lpszClassName
        , L"asari"
        , WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE
        , CW_USEDEFAULT
        , CW_USEDEFAULT
        , CW_USEDEFAULT
        , CW_USEDEFAULT
        , nullptr
        , nullptr
        , instance
        , nullptr);

    if (m_window_proc_error)
    {
        std::rethrow_exception(std::move(m_window_proc_error));
    }

    if (!window)
    {
        ASR_THROW(get_last_error(), "win", "CreateWindowExW failed");
    }

    register_for_input();

    m_renderer_backend = make_direct2d_renderer(window);
    m_renderer_backend->initialize();

    ::ShowWindow(window, SW_SHOW);
    ::UpdateWindow(window);

    m_observer->on_ready();

    MSG msg {};
    while (auto const result = ::GetMessageW(&msg, nullptr, 0, 0))
    {
        ASR_ASSERT(!m_window_proc_error);

        if (result == -1)
        {
            ASR_THROW(get_last_error(), "win", "GetMessageW failed");
        }

        if (msg.hwnd != window)
        {
        }

        ASR_DISCARD_RESULT ::TranslateMessage(&msg);
        ASR_DISCARD_RESULT ::DispatchMessageW(&msg);

        if (m_window_proc_error)
        {
            auto e = std::move(m_window_proc_error);
            m_window_proc_error = std::exception_ptr {};

            ASR_THROW(std::move(e), "win", "exception in window proc");
        }
    }

    if (msg.message != WM_QUIT)
    {
        ASR_ABORT_UNREACHABLE();
    }
}

renderer_backend& application::get_renderer_backend() const noexcept
{
    return *m_renderer_backend;
}

application& application::get_window_instance(application* const value) noexcept
{
    thread_local application* instance = value;
    return *instance;
}

void application::init(HWND const window) noexcept
{
    ASR_ASSERT(!m_initialized);

    m_window = window;
    m_input_manager->set_window(m_window);
    m_initialized = true;
}

LRESULT __stdcall application::top_level_window_proc(
      HWND   const hWnd
    , UINT   const message
    , WPARAM const wParam
    , LPARAM const lParam) noexcept
{
    __try
    {
        auto& instance = get_window_instance();

        if (!instance.m_initialized)
        {
            instance.init(hWnd);
        }

        return instance.window_proc(message, wParam, lParam);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        DWORD id {};
        ::CreateThread(
              nullptr
            , 0
            , [](void*) noexcept -> DWORD { fail_fast(); }
            , nullptr
            , STACK_SIZE_PARAM_IS_A_RESERVATION
            , &id);
        ::Sleep(10000);
    }

    fail_fast();
}

template <>
std::optional<LRESULT> application::window_proc<WM_PAINT>(WPARAM, LPARAM)
{
    ::ValidateRect(m_window, nullptr);
    return 0;
}

template <>
std::optional<LRESULT> application::window_proc<WM_ERASEBKGND>(WPARAM, LPARAM)
{
    return 1;
}

template <>
std::optional<LRESULT> application::window_proc<WM_ACTIVATE>(WPARAM const wParam, LPARAM)
{
    auto const state = HIWORD(wParam);
    auto const flags = LOWORD(wParam);

    if (flags != WA_INACTIVE)
    {
        m_input_manager->set_focus(true);
    }

    return std::nullopt;
}

template <>
std::optional<LRESULT> application::window_proc<WM_CREATE>(WPARAM, LPARAM)
{
    return 0;
}

template <>
std::optional<LRESULT> application::window_proc<WM_DESTROY>(WPARAM, LPARAM)
{
    ::PostQuitMessage(0);
    return 0;
}

template <>
std::optional<LRESULT> application::window_proc<WM_CLOSE>(WPARAM, LPARAM)
{
    return std::nullopt;
}

template <>
std::optional<LRESULT> application::window_proc<WM_SETCURSOR>(WPARAM, LPARAM)
{
    return std::nullopt;
}

void on_raw_mouse(RAWINPUTHEADER const&, RAWMOUSE const&)
{
}

void on_raw_keyboard(RAWINPUTHEADER const&, RAWKEYBOARD const& kb)
{
    constexpr USHORT KEY_MAKE             = 0x00;
    constexpr USHORT KEY_BREAK            = 0x01;
    constexpr USHORT KEY_E0               = 0x02;
    constexpr USHORT KEY_E1               = 0x04;
    constexpr USHORT KEY_TERMSRV_SET_LED  = 0x08;
    constexpr USHORT KEY_TERMSRV_SHADOW   = 0x10;
    constexpr USHORT KEY_TERMSRV_VKPACKET = 0x20;
    constexpr USHORT KEY_RIM_VKEY         = 0x40;

    ASR_LOGA("[win] WM_INPUT %hX %hX %hX %lX", kb.MakeCode, kb.VKey, kb.Flags, kb.ExtraInformation);
}

template <>
std::optional<LRESULT> application::window_proc<WM_INPUT>(WPARAM const wParam, LPARAM const lParam)
{
    switch (wParam & 0xFF)
    {
    case RIM_INPUT:
        break;
    case RIM_INPUTSINK:
        break;
    default:
        ASR_ABORT_UNREACHABLE();
        break;
    }

    alignas(16) char buffer[64] {};

    UINT size = sizeof(buffer);
    auto const result = ::GetRawInputData(
          reinterpret_cast<HRAWINPUT>(lParam)
        , RID_INPUT
        , buffer
        , &size
        , sizeof(RAWINPUTHEADER));

    if (result == static_cast<UINT>(-1))
    {
        ASR_ABORT_UNREACHABLE();
    }

    auto const header = *reinterpret_cast<RAWINPUTHEADER const*>(buffer);
    auto const offset = buffer + sizeof(RAWINPUTHEADER);

    switch (header.dwType)
    {
    case RIM_TYPEMOUSE:
        on_raw_mouse(header, *reinterpret_cast<RAWMOUSE const*>(offset));
        break;
    case RIM_TYPEKEYBOARD:
        on_raw_keyboard(header, *reinterpret_cast<RAWKEYBOARD const*>(offset));
        break;
    case RIM_TYPEHID:
        break;
    default:
        ASR_ABORT_UNREACHABLE();
    }

    return std::nullopt;
}

struct key_info
{
    uint16_t repeat;
    uint8_t  scan_code;
    uint8_t  reserved;
    bool     is_extended;
    bool     context;
    bool     previous;
    bool     transition;
};

constexpr key_info make_key_info(LPARAM const lParam) noexcept
{
    auto const get = [=](auto const mask, auto const shift) noexcept {
        return static_cast<uint16_t>((lParam & mask) >> shift);
    };

    key_info result {};

    result.repeat      = get(0b0000'0000'0000'0000'1111'1111'1111'1111u,  0u);
    result.scan_code   = static_cast<uint8_t>(get(0b0000'0000'1111'1111'0000'0000'0000'0000u, 16u));
    result.is_extended = !!get(0b0000'0001'0000'0000'0000'0000'0000'0000u, 24u);
    result.reserved    = static_cast<uint8_t>(get(0b0001'1110'0000'0000'0000'0000'0000'0000u, 25u));
    result.context     = !!get(0b0010'0000'0000'0000'0000'0000'0000'0000u, 29u);
    result.previous    = !!get(0b0100'0000'0000'0000'0000'0000'0000'0000u, 30u);
    result.transition  = !!get(0b1000'0000'0000'0000'0000'0000'0000'0000u, 31u);

    return result;
}

std::optional<LRESULT> on_keyboard(WPARAM const wParam, LPARAM const lParam)
{
    auto const info = make_key_info(lParam);

    ASR_LOGA("[win] WM_KEY %zX %hX %hX %hX %hX %hX %hX %hX"
        , static_cast<size_t>(wParam)
        , info.repeat
        , info.scan_code
        , info.is_extended
        , info.reserved
        , info.context
        , info.previous
        , info.transition);

    return std::nullopt;
}

template <>
std::optional<LRESULT> application::window_proc<WM_KEYDOWN>(WPARAM const wParam, LPARAM const lParam)
{
    return on_keyboard(wParam, lParam);
}

template <>
std::optional<LRESULT> application::window_proc<WM_KEYUP>(WPARAM const wParam, LPARAM const lParam)
{
    return on_keyboard(wParam, lParam);
}

template <>
std::optional<LRESULT> application::window_proc<WM_CHAR>(WPARAM const wParam, LPARAM const lParam)
{
    auto const info = make_key_info(lParam);

    ASR_LOGA("[win] WM_CHAR %zX %hX %hX %hX %hX %hX %hX %hX"
        , static_cast<size_t>(wParam)
        , info.repeat
        , info.scan_code
        , info.is_extended
        , info.reserved
        , info.context
        , info.previous
        , info.transition);

    keyboard_event event {};
    event.character = static_cast<int>(wParam);

    m_observer->on_system_event(event);

    return std::nullopt;
}

LRESULT application::window_proc(
      UINT   const message
    , WPARAM const wParam
    , LPARAM const lParam) noexcept
try
{
    ASR_ASSERT(m_initialized);

    #define ASR_DISPATCH_MSG(x) case x : return window_proc<x>(wParam, lParam)

    auto const result = std::invoke([&]() -> std::optional<LRESULT> {
        switch (message)
        {
        ASR_DISPATCH_MSG(WM_CREATE);
        ASR_DISPATCH_MSG(WM_DESTROY);
        ASR_DISPATCH_MSG(WM_PAINT);
        ASR_DISPATCH_MSG(WM_ERASEBKGND);
        ASR_DISPATCH_MSG(WM_ACTIVATE);
        ASR_DISPATCH_MSG(WM_CLOSE);
        ASR_DISPATCH_MSG(WM_SETCURSOR);
        ASR_DISPATCH_MSG(WM_INPUT);
        ASR_DISPATCH_MSG(WM_KEYDOWN);
        ASR_DISPATCH_MSG(WM_KEYUP);
        ASR_DISPATCH_MSG(WM_CHAR);
        default:
            break;
        }

        if (message != WM_NCHITTEST && message != WM_MOUSEMOVE && message != WM_NCMOUSEMOVE)
        {
            ASR_LOGA("[win] message % 5u : 0x%p 0x%p"
                , message
                , reinterpret_cast<void*>(wParam)
                , reinterpret_cast<void*>(lParam));
        }

        return std::nullopt;
    });

    #undef ASR_DISPATCH_MSG

    return result
        ? *result
        : ::DefWindowProcW(m_window, message, wParam, lParam);
}
catch (...)
{
    ASR_ASSERT(!m_window_proc_error);
    m_window_proc_error = std::current_exception();

    return ::DefWindowProcW(m_window, message, wParam, lParam);
}

} // namespace asr::win

namespace asr
{

void fail_fast()
{
    __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

application_observer::~application_observer() = default;

application::~application() = default;

std::unique_ptr<application> make_application(application_options options)
{
    ASR_ASSERT(dynamic_cast<win::text_input_manager*>(options.ime));
    ASR_ASSERT(options.observer);

    return std::make_unique<win::application>(std::move(options));
}

void intial_setup()
{
    ASR_LOGA("[win] initialization");

    if (auto const hr = ::CoInitializeSecurity(
              nullptr                     // descriptor
            , -1                          // COM authentication
            , nullptr                     // authentication services
            , nullptr                     // reserved
            , RPC_C_AUTHN_LEVEL_DEFAULT   // default authentication
            , RPC_C_IMP_LEVEL_IMPERSONATE // default impersonation
            , nullptr                     // authentication info
            , EOAC_NONE                   // additional capabilities
            , nullptr                     /* reserved */);
        FAILED(hr))
    {
        if (hr == RPC_E_TOO_LATE)
        {
            ASR_ABORT_UNREACHABLE();
        }
    }

    if (auto const hr = ::CoInitializeEx(
              nullptr
            , COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        hr != S_OK)
    {
        ASR_ASSERT(hr != S_FALSE);
        ASR_THROW(win::make_com_error(hr), "win", "CoInitializeEx failed");
    }

    IGlobalOptions* options {};
    if (auto const hr = ::CoCreateInstance(
              CLSID_GlobalOptions
            , nullptr
            , CLSCTX_INPROC_SERVER
            , IID_PPV_ARGS(&options));
        FAILED(hr))
    {
        ASR_THROW(win::make_com_error(hr), "main", "CoCreateInstance for IGlobalOptions failed");
    }

    options->Set(COMGLB_EXCEPTION_HANDLING, COMGLB_EXCEPTION_DONOT_HANDLE_ANY);
    options->Release();
}

} // namespace asr
