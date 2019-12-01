#define _CONTAINER_DEBUG_LEVEL 0
#define _ITERATOR_DEBUG_LEVEL  0

#include "win_common.h"

#include "win_main.h"

#include <WerApi.h>
#include <Psapi.h>

#include <iterator>
#include <type_traits>
#include <array>
#include <algorithm>
#include <string_view>
#include <memory>

using namespace std::string_view_literals;

extern "C" IMAGE_DOS_HEADER __ImageBase;

extern "C" int __cdecl _except_handler3();
extern "C" int __cdecl _except_handler4();

constexpr auto stub_target = LR"(asari.dll)";

asr::win::main_module_interface module_interface {};

//------------------------------------------------------------------------------
[[noreturn]] void fatal_exit()
{
    if (::IsDebuggerPresent())
    {
        __debugbreak();
    }

    __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

//------------------------------------------------------------------------------
LONG __stdcall exception_filter(EXCEPTION_POINTERS* const e)
{
    if (::IsDebuggerPresent())
    {
        __debugbreak();
    }

    if (module_interface.fatal_error)
    {
        module_interface.fatal_error(e);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

//------------------------------------------------------------------------------
wchar_t const* get_command_line()
{
    auto const original_command_line = ::GetCommandLineW();

    int n = 0;
    for (auto it = original_command_line; it && *it; ++it)
    {
        if (*it == L'"')
        {
            ++n;
        }

        if (n == 2)
        {
            return ++it;
        }
    }

    return original_command_line;
}

//------------------------------------------------------------------------------
struct virtual_memory_deleter
{
    using pointer = char*;
    void operator()(pointer const p) const
    {
        ::VirtualFree(p, 0, MEM_RELEASE);
    }
};

using unique_memory = std::unique_ptr<char*, virtual_memory_deleter>;

std::pair<wchar_t const*, unique_memory> get_target_full_path()
{
    constexpr auto namespace_prefix = LR"(\\.\GLOBALROOT\)"sv;
    constexpr auto absolute_prefix  = LR"(\\?\)"sv;

    constexpr size_t max_ntfs_path = 0x8000;

    using mapped_buffer_t = std::array<wchar_t, max_ntfs_path + size(namespace_prefix)>;
    using path_buffer_t   = std::array<wchar_t, max_ntfs_path + size(absolute_prefix)>;

    constexpr auto required_bytes = sizeof(mapped_buffer_t) + sizeof(path_buffer_t);

    auto const buffer_pool = reinterpret_cast<char*>(::VirtualAlloc(
          nullptr
        , required_bytes
        , MEM_COMMIT | MEM_RESERVE
        , PAGE_READWRITE));

    if (!buffer_pool)
    {
        fatal_exit();
    }

    auto& mapped_buffer = *(new (buffer_pool + 0)                       mapped_buffer_t);
    auto& path_buffer   = *(new (buffer_pool + sizeof(mapped_buffer_t)) path_buffer_t);

    constexpr auto offset = size(namespace_prefix) - 1;

    auto const mapped_name_length = ::K32GetMappedFileNameW(
        ::GetCurrentProcess()
        , &__ImageBase
        , data(mapped_buffer)                    + offset
        , static_cast<DWORD>(size(mapped_buffer) - offset));

    if (!mapped_name_length)
    {
        fatal_exit();
    }

    {
        size_t i = 0;
        for (auto const c : namespace_prefix)
        {
            mapped_buffer[i++] = c;
        }
    }

    auto const handle = ::CreateFileW(
          data(mapped_buffer)
        , FILE_READ_ATTRIBUTES
        , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
        , nullptr
        , OPEN_EXISTING
        , 0
        , nullptr);

    if (!handle || handle == INVALID_HANDLE_VALUE)
    {
        fatal_exit();
    }

    auto const path_len = ::GetFinalPathNameByHandleW(
          handle
        , data(path_buffer)
        , static_cast<DWORD>(size(path_buffer))
        , 0);

    if (!path_len)
    {
        fatal_exit();
    }

    if (!::CloseHandle(handle))
    {
        fatal_exit();
    }

    auto path_root = data(path_buffer) + path_len;

    for (size_t i = 0; i < path_len; ++i)
    {
        if (*path_root == L'\\')
        {
            ++path_root;
            break;
        }

        --path_root;
    }

    for (auto it = stub_target; *it; ++it)
    {
        *path_root++ = *it;
    }

    *path_root = L'\0';

    return std::pair(data(path_buffer), unique_memory(buffer_pool));
}

//------------------------------------------------------------------------------
int launch_target()
{
    auto const module = std::invoke([] {
        auto const [target, target_memory] = get_target_full_path();
        return ::LoadLibraryExW(target, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    });

    if (!module)
    {
        fatal_exit();
    }

    auto const entry = reinterpret_cast<asr::win::load_main_module_t>(reinterpret_cast<uintptr_t>(
        ::GetProcAddress(module, asr::win::load_main_module)));

    if (!entry)
    {
        fatal_exit();
    }

    __try
    {
        module_interface.version = asr::win::module_version::current;

        if (auto const e = entry(&module_interface))
        {
            return e;
        }

        auto const result = module_interface.dll_main(get_command_line());

        if (!::FreeLibrary(module))
        {
            fatal_exit();
        }

        return result;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        fatal_exit();
    }
}

void set_crash_on_crashes()
{
    HMODULE module {};
    if (!::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT , L"kernel32.dll", &module))
    {
        return;
    }

    auto const get = ::GetProcAddress(module, "GetProcessUserModeExceptionPolicy");
    auto const set = ::GetProcAddress(module, "SetProcessUserModeExceptionPolicy");

    if (!get || !set)
    {
        return;
    }

    using get_t = BOOL (__stdcall*)(LPDWORD lpFlags);
    using set_t = BOOL (__stdcall*)(DWORD dwFlags);

    auto const get_flags = static_cast<get_t>(reinterpret_cast<void*>(get));
    auto const set_flags = static_cast<set_t>(reinterpret_cast<void*>(set));

    DWORD flags {};
    if (!get_flags(&flags))
    {
        return;
    }

    constexpr DWORD EXCEPTION_SWALLOWING = 0x1;

    set_flags(flags & ~EXCEPTION_SWALLOWING);
}

void hook_unhandled_exception_filter()
{
    auto const p = &SetUnhandledExceptionFilter;

#ifdef _M_IX86
    // Code for x86:
    // 33 C0                xor         eax,eax
    // C2 04 00             ret         4
    constexpr unsigned char code[] {0x33, 0xC0, 0xC2, 0x04, 0x00};
#elif _M_X64
    // 33 C0                xor         eax,eax
    // C3                   ret
    constexpr unsigned char code[] {0x33, 0xC0, 0xC3};
#else
#   error "code only works for x86 and x64"
#endif

    __try
    {
        size_t written {};
        (void)::WriteProcessMemory(
            ::GetCurrentProcess()
          , p
          , code
          , sizeof(code)
          , &written);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // best effort -- ignore access violations
    }
}

//------------------------------------------------------------------------------
void set_process_mitigations()
{
    ::SetProcessDEPPolicy(PROCESS_DEP_ENABLE | PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION);

    ::SetErrorMode(SEM_FAILCRITICALERRORS);

    ::HeapSetInformation(nullptr, HEAP_INFORMATION_CLASS::HeapEnableTerminationOnCorruption, nullptr, 0);

    ::SetUnhandledExceptionFilter(exception_filter);
    hook_unhandled_exception_filter();

    set_crash_on_crashes();

    ::SetSearchPathMode(BASE_SEARCH_PATH_PERMANENT | BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE);
    ::SetDllDirectoryW(L"");

    ::WerSetFlags(WER_FAULT_REPORTING_NO_UI | WER_FAULT_REPORTING_DURABLE);
}

//------------------------------------------------------------------------------
int __stdcall main()
{
    set_process_mitigations();
    auto const result = launch_target();
    ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(result));
}
