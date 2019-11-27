#include "win_app.h"

#include "win_common.h"
#include "win_ime.h"

#include "common.h"

namespace asr::win
{

namespace
{

constexpr bool to_hex(
      char*       const out
    , size_t      const out_size
    , char const* const data
    , size_t      const data_size) noexcept
{
    constexpr char table[] {
          '0', '1', '2', '3'
        , '4', '5', '6', '7'
        , '8', '9', 'A', 'B'
        , 'C', 'D', 'E', 'F'
    };

    std::string_view const sv {data, data_size};

    auto it   = out;
    auto last = it + out_size;

    for (auto const c : sv)
    {
        if (it == last || it + 1 == last)
        {
            return false;
        }

        *it++ = table[(c & 0xF0) >> 4];
        *it++ = table[(c & 0x0F) >> 0];
    }

    return true;
}

constexpr bool widen(
      wchar_t*    const out
    , size_t      const out_size
    , char const* const data
    , size_t      const data_size) noexcept
{
    std::string_view const sv {data, data_size};

    auto it   = out;
    auto last = it + out_size;

    for (auto const c : sv)
    {
        if (it == last)
        {
            return false;
        }

        auto const w = static_cast<wchar_t>(c & 0x7F);

        *it++ = w ? w : L' ';
    }

    return true;
}

template <typename T>
auto to_hex(T const& bytes) noexcept
{
    std::array<char, sizeof(T) * 2 + 1> result {};

    to_hex(
          data(result)
        , size(result)
        , reinterpret_cast<char const*>(std::addressof(bytes))
        , sizeof(bytes));

    return result;
}

template <typename Container>
std::wstring widen(Container const& narrow) noexcept
{
    using std::size;
    using std::data;

    std::wstring result;
    result.resize(narrow.size());

    widen(data(result), size(result), data(narrow), size(narrow));

    return result;
}

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

} // namespace

application::application(application_options options)
    : m_input_manager(static_cast<text_input_manager*>(options.ime))
{
    ASR_ASSERT(m_input_manager);
    ASR_ASSERT(options.on_ready);

    get_window_instance(this);

    auto const instance = reinterpret_cast<HINSTANCE>(&__ImageBase);

    LUID id {};
    if (!::AllocateLocallyUniqueId(&id))
    {
        ASR_THROW(std::errc::interrupted, "main", "AllocateLocallyUniqueId failed");
    }

    auto const string_id = widen(to_hex(id));

    WNDCLASSEXW wc {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = top_level_window_proc;
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
        , L"asari"
        , WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS //WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU
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

    ASR_ASSERT(window);

    ::ShowWindow(window, SW_SHOW);
    ::UpdateWindow(window);

    options.on_ready();

    MSG msg {};
    while (auto const result = ::GetMessageW(&msg, nullptr, 0, 0))
    {
        ASR_ASSERT(!m_window_proc_error);

        if (result == -1)
        {
            ASR_THROW(std::errc::interrupted, "main_window", "GetMessageW failed");
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

            ASR_THROW(std::move(e), "main_window", "exception in window proc");
        }
    }

    if (msg.message != WM_QUIT)
    {
        ASR_ABORT_UNREACHABLE();
    }
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

LRESULT application::window_proc(
      UINT   const message
    , WPARAM const wParam
    , LPARAM const lParam) noexcept
try
{
    ASR_ASSERT(m_initialized);

    #define ASR_DISPATCH_MSG(x) case x : return window_proc<x>(wParam, lParam)

    ASR_LOGA("[win] message %u : %zx %zx", message, wParam, lParam);

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
        default:
            break;
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

application::~application() = default;

std::unique_ptr<application> make_application(application_options options)
{
    ASR_ASSERT(dynamic_cast<win::text_input_manager*>(options.ime));

    return std::make_unique<win::application>(std::move(options));
}

void intial_setup()
{
    ASR_LOGA("[win] initialization");

    //::CoInitializeSecurity

    if (auto const hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        hr != S_OK)
    {
        ASR_ASSERT(hr != S_FALSE);
        ASR_THROW(std::errc::interrupted, "main", "CoInitializeEx failed");
    }

    IGlobalOptions* options {};
    if (auto const hr = ::CoCreateInstance(CLSID_GlobalOptions, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&options));
        FAILED(hr))
    {
        ASR_THROW(std::errc::interrupted, "main", "CoCreateInstance for IGlobalOptions failed");
    }

    options->Set(COMGLB_EXCEPTION_HANDLING, COMGLB_EXCEPTION_DONOT_HANDLE_ANY);
    options->Release();
}

} // namespace asr
