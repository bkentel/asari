#include "win_main.h"

#include "win_common.h"

#include "platform/app.h"

#include "common.h"

#include <csignal>

namespace asr::win
{

namespace
{

void alloc_console()
{
    {
        DWORD id {};

        auto const handle = ::CreateThread(
              nullptr
            , 0
            , [](void*) noexcept -> DWORD { return ::AllocConsole() & 0; }
            , nullptr
            , STACK_SIZE_PARAM_IS_A_RESERVATION
            , &id);

        if (handle)
        {
            ::WaitForSingleObject(handle, INFINITE);
        }
    }

    FILE* dummy {};

    (void)freopen_s(&dummy, "CONIN$",  "r", stdin);
    (void)freopen_s(&dummy, "CONOUT$", "w", stdout);
    (void)freopen_s(&dummy, "CONOUT$", "w", stderr);
}

[[noreturn]] void fatal_error(EXCEPTION_POINTERS* const e) noexcept
{
    if (::IsDebuggerPresent())
    {
        __debugbreak();
    }

    ASR_LOGE("[win] module fatal error");

    if (e)
    {
    }

    __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

int dll_main(wchar_t const* const command_line) noexcept
{
    ASR_LOGA("[win] module main '%ls'", command_line);

    std::vector<std::string_view> args;

    return asr::main(args);
}

int init(main_module_interface& out)
{
    alloc_console();

    ASR_LOGA("[win] module init %ju", out.version);

    if (out.version != module_version::current)
    {
        return 2;
    }

    out = main_module_interface {};

    out.version     = module_version::current;
    out.dll_main    = &dll_main;
    out.fatal_error = &fatal_error;

    return 0;
}

std::atomic_int processing_failure = 0;

enum class failure_type : int
{
      unknown
    , abort
    , terminate
    , new_failure
    , invalid_parameter
    , purecall
    , runtime_check
};

[[noreturn]] void on_failure(failure_type) noexcept
{
    if (auto const n = ++processing_failure;
        n == 1)
    {
        signal(SIGABRT, [](int) noexcept {
            on_failure(failure_type::abort);
        });
    }

    fatal_error(nullptr);
}

void set_fatal_handlers() noexcept
{
    _set_abort_behavior(_CALL_REPORTFAULT, ~(uint32_t {0}));

    signal(SIGABRT, [](int) noexcept {
        on_failure(failure_type::abort);
    });

    std::set_terminate([]() noexcept {
        on_failure(failure_type::terminate);
    });

    _set_new_mode(1);

    std::set_new_handler([]() noexcept {
        on_failure(failure_type::new_failure);
    });

    _set_invalid_parameter_handler([](auto, auto, auto, auto, auto) noexcept {
        on_failure(failure_type::invalid_parameter);
    });

    _set_purecall_handler([]() noexcept {
        on_failure(failure_type::purecall);
    });

    {
        BOOL policy = FALSE;
        ::SetUserObjectInformationW(
              ::GetCurrentProcess()
            , UOI_TIMERPROC_EXCEPTION_SUPPRESSION
            , &policy
            , sizeof(policy));
    }

    #ifdef __MSVC_RUNTIME_CHECKS

    _CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);

    _CrtSetReportHookW2(_CRT_RPTHOOK_INSTALL, [](int const type, wchar_t* const message, int*) noexcept -> int {
        ASR_LOGE("[win] runtime check (%d): '%ls'", type, message);
        on_failure(failure_type::runtime_check);
    });

    #endif
}

} // namespace

} // namespace asr::win

BOOL __stdcall DllMain(
      [[maybe_unused]] HINSTANCE const instance
    , [[maybe_unused]] DWORD     const reason
    , [[maybe_unused]] LPVOID    const reserved)
try
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        asr::win::set_fatal_handlers();
    }

    return TRUE;
}
catch (...)
{
    return FALSE;
}

extern "C" __declspec(dllexport) int __cdecl init(
    asr::win::main_module_interface* const out) noexcept
try
{
    if (!out)
    {
        ASR_ABORT_UNREACHABLE();
        return 1;
    }

    return asr::win::init(*out);
}
catch (...)
{
    return -1;
}
