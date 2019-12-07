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

BOOL __stdcall GetProcessUserModeExceptionPolicy(LPDWORD lpFlags);
BOOL __stdcall SetProcessUserModeExceptionPolicy(DWORD dwFlags);

namespace asr::win
{

namespace
{

template <typename T, auto Result>
struct dummy_api;

template <typename R, typename... Args, auto Result>
struct dummy_api<R __stdcall (Args...), Result>
{
    using type = R (__stdcall*)(Args...);
    constexpr operator type() const noexcept
    {
        return &invoke;
    }

    constexpr static R __stdcall invoke(Args...)
    {
        return Result;
    }
};

template <typename T, auto Fallback>
T* get_function(HMODULE const module, char const* const function)
{
    auto const p = ::GetProcAddress(module, function);
    if (!p)
    {
        return dummy_api<T, Fallback>();
    }

    return static_cast<T*>(reinterpret_cast<void*>(p));
}

inline auto GetThreadInformation              = decltype(&::GetThreadInformation) {};
inline auto SetThreadInformation              = decltype(&::SetThreadInformation) {};
inline auto GetProcessUserModeExceptionPolicy = decltype(&::GetProcessUserModeExceptionPolicy) {};
inline auto SetProcessUserModeExceptionPolicy = decltype(&::SetProcessUserModeExceptionPolicy) {};
inline auto GetProcessMitigationPolicy        = decltype(&::GetProcessMitigationPolicy) {};
inline auto SetProcessMitigationPolicy        = decltype(&::SetProcessMitigationPolicy) {};

void get_optional_apis()
{
    HMODULE module {};
    ::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"kernel32.dll", &module);

    #define ASR_GET_F(F, RESULT) F = get_function<decltype(::F), RESULT>(module, #F)

    ASR_GET_F(GetThreadInformation,              FALSE);
    ASR_GET_F(SetThreadInformation,              FALSE);
    ASR_GET_F(GetProcessUserModeExceptionPolicy, FALSE);
    ASR_GET_F(SetProcessUserModeExceptionPolicy, FALSE);
    ASR_GET_F(GetProcessMitigationPolicy,        FALSE);
    ASR_GET_F(SetProcessMitigationPolicy,        FALSE);

    #undef ASR_GET_F
}

} // namespace

} // namespace asr::win

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
[[nodiscard]] wchar_t const* get_command_line()
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

[[nodiscard]] std::pair<wchar_t const*, unique_memory> get_target_full_path(
    wchar_t const* const filename)
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

    if (filename)
    {
        for (auto it = filename; *it; ++it)
        {
            *path_root++ = *it;
        }

        *path_root = L'\0';
    }

    return std::pair(data(path_buffer), unique_memory(buffer_pool));
}

//------------------------------------------------------------------------------
int launch_target()
{
    auto const module = std::invoke([] {
        auto const [target, target_memory] = get_target_full_path(stub_target);
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

//------------------------------------------------------------------------------
void set_crash_on_crashes()
{
    DWORD flags {};
    if (!asr::win::GetProcessUserModeExceptionPolicy(&flags))
    {
        return;
    }

    constexpr DWORD EXCEPTION_SWALLOWING = 0x1;

    asr::win::SetProcessUserModeExceptionPolicy(flags & ~EXCEPTION_SWALLOWING);
}

//------------------------------------------------------------------------------
void hook_unhandled_exception_filter()
{
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

    auto const handle = ::GetCurrentThread();

    DWORD thread_policy = THREAD_DYNAMIC_CODE_ALLOW;
    asr::win::SetThreadInformation(
          handle
        , THREAD_INFORMATION_CLASS::ThreadDynamicCodePolicy
        , &thread_policy
        , sizeof(thread_policy));

    auto const p = &SetUnhandledExceptionFilter;

    SIZE_T written {};
    ::WriteProcessMemory(
          ::GetCurrentProcess()
        , p
        , code
        , sizeof(code)
        , &written);

    thread_policy = 0;
    asr::win::SetThreadInformation(
          handle
        , THREAD_INFORMATION_CLASS::ThreadDynamicCodePolicy
        , &thread_policy
        , sizeof(thread_policy));
}

//------------------------------------------------------------------------------
void set_process_mitigations()
{
    ::SetProcessDEPPolicy(PROCESS_DEP_ENABLE | PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION);

    ::SetErrorMode(SEM_FAILCRITICALERRORS);

    ::HeapSetInformation(nullptr, HEAP_INFORMATION_CLASS::HeapEnableTerminationOnCorruption, nullptr, 0);

    ::SetUnhandledExceptionFilter(exception_filter);
    hook_unhandled_exception_filter();

    PROCESS_MITIGATION_DYNAMIC_CODE_POLICY policy;
    ::RtlSecureZeroMemory(&policy, sizeof(policy));
    policy.ProhibitDynamicCode = 1;

    asr::win::SetProcessMitigationPolicy(PROCESS_MITIGATION_POLICY::ProcessDynamicCodePolicy, &policy, sizeof(policy));

    set_crash_on_crashes();

    ::SetSearchPathMode(BASE_SEARCH_PATH_PERMANENT | BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE);
    ::SetDllDirectoryW(L"");

    ::WerSetFlags(WER_FAULT_REPORTING_NO_UI | WER_FAULT_REPORTING_DURABLE);
}

//------------------------------------------------------------------------------
int relaunch_protected()
{
    SIZE_T size {};
    ::InitializeProcThreadAttributeList(nullptr, 1, 0, &size);

    alignas (16) char buffer[64];
    ::RtlSecureZeroMemory(&buffer, sizeof(buffer));

    auto const attribute_list = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(buffer);

    ::InitializeProcThreadAttributeList(attribute_list, 1, 0, &size);

    constexpr DWORD64 mitigations =
          PROCESS_CREATION_MITIGATION_POLICY_DEP_ENABLE
        | PROCESS_CREATION_MITIGATION_POLICY_SEHOP_ENABLE
        | PROCESS_CREATION_MITIGATION_POLICY_HEAP_TERMINATE_ALWAYS_ON
        | PROCESS_CREATION_MITIGATION_POLICY_BOTTOM_UP_ASLR_ALWAYS_ON
        | PROCESS_CREATION_MITIGATION_POLICY_HIGH_ENTROPY_ASLR_ALWAYS_ON
        | PROCESS_CREATION_MITIGATION_POLICY_STRICT_HANDLE_CHECKS_ALWAYS_ON
        //| PROCESS_CREATION_MITIGATION_POLICY_WIN32K_SYSTEM_CALL_DISABLE_ALWAYS_ON
        | PROCESS_CREATION_MITIGATION_POLICY_EXTENSION_POINT_DISABLE_ALWAYS_ON
        | PROCESS_CREATION_MITIGATION_POLICY_FORCE_RELOCATE_IMAGES_ALWAYS_ON_REQ_RELOCS
        | PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON_ALLOW_OPT_OUT
        | PROCESS_CREATION_MITIGATION_POLICY_CONTROL_FLOW_GUARD_ALWAYS_ON
        //| PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON
        //| PROCESS_CREATION_MITIGATION_POLICY_FONT_DISABLE_ALWAYS_ON
        | PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_REMOTE_ALWAYS_ON
        //|PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_LOW_LABEL_ALWAYS_ON
        | PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_PREFER_SYSTEM32_ALWAYS_ON;

    auto mitigation_value = mitigations;

    ::UpdateProcThreadAttribute(
          attribute_list
        , 0
        , PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY
        , &mitigation_value
        , sizeof(mitigation_value)
        , nullptr
        , nullptr);

    wchar_t title[] = L"asari";

    STARTUPINFOEXW si;
    ::RtlSecureZeroMemory(&si, sizeof(si));

    si.StartupInfo.cb      = sizeof(si);
    si.lpAttributeList     = attribute_list;
    si.StartupInfo.lpTitle = title;

    PROCESS_INFORMATION pi;
    ::RtlSecureZeroMemory(&pi, sizeof(pi));

    auto const [path, path_storage] = get_target_full_path(nullptr);

    auto const ok = ::CreateProcessW(
          path
        , ::GetCommandLineW()
        , nullptr
        , nullptr
        , FALSE
        , EXTENDED_STARTUPINFO_PRESENT | CREATE_SUSPENDED
        , nullptr
        , nullptr
        , reinterpret_cast<LPSTARTUPINFOW>(&si)
        , &pi);

    if (!ok)
    {
        return false;
    }

    ::ResumeThread(pi.hThread);

    ::CloseHandle(pi.hThread);
    ::CloseHandle(pi.hProcess);

    return true;
}

//------------------------------------------------------------------------------
[[nodiscard]] bool is_protected()
{
    STARTUPINFOW info;
    ::GetStartupInfoW(&info);

    // we've already relaunched once
    if (info.lpTitle == L"asari"sv)
    {
        return true;
    }

    PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY policy;
    if (!asr::win::GetProcessMitigationPolicy(
          ::GetCurrentProcess()
        , PROCESS_MITIGATION_POLICY::ProcessExtensionPointDisablePolicy
        , &policy
        , sizeof(policy)))
    {
        return false;
    }

    return !!policy.DisableExtensionPoints;
}

//------------------------------------------------------------------------------
int launch()
{
    if (!is_protected())
    {
        return relaunch_protected();
    }

    set_process_mitigations();
    return launch_target();
}

//------------------------------------------------------------------------------
int __stdcall main()
{
    asr::win::get_optional_apis();
    return ::TerminateProcess(::GetCurrentProcess(), launch());
}
