#include "win_common.h"

#include "platform/app.h"

#include "common.h"

namespace
{

void alloc_console() noexcept
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

} // namespace

BOOL __stdcall DllMain(
      [[maybe_unused]] HINSTANCE const instance
    , [[maybe_unused]] DWORD     const reason
    , [[maybe_unused]] LPVOID    const reserved)
try
{
    return TRUE;
}
catch (...)
{
    return FALSE;
}

__declspec(dllexport) int __stdcall dll_main(
    [[maybe_unused]] wchar_t const* const cmd_line) noexcept
try
{
    alloc_console();

    ASR_LOGA("[win] main '%ls'", cmd_line);

    std::vector<std::string_view> args;
    return asr::main(args);
}
catch (...)
{
    return -1;
}
