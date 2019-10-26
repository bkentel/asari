#ifdef _WIN32

#include <sdkddkver.h>
#include <Windows.h>

BOOL __stdcall DllMain(
      [[maybe_unused]] HINSTANCE instance
    , [[maybe_unused]] DWORD     reason
    , [[maybe_unused]] LPVOID    reserved)
try
{
    return TRUE;
}
catch (...)
{
    return FALSE;
}

extern "C" __declspec(dllexport) int __stdcall dll_main(wchar_t const* const cmd_line) noexcept
try
{
    return 0;
}
catch (...)
{
    return -1;
}

#endif
