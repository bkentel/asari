#include "win_util.h"

#include "win_error.h"

#include <winternl.h>

namespace asr::win
{

namespace
{

windows_version get_win_version()
{
    DWORD dummy {};
    auto const bytes = ::GetFileVersionInfoSizeExW(
          FILE_VER_GET_NEUTRAL
        , L"ntdll.dll"
        , &dummy);

    if (!bytes)
    {
        ASR_THROW(get_last_error(), "win", "GetFileVersionInfoSizeExW failed for ntdll");
    }

    auto const buffer = std::make_unique<char[]>(bytes);

    if (!::GetFileVersionInfoExW(
          FILE_VER_GET_NEUTRAL
        , L"ntdll.dll"
        , 0
        , bytes
        , buffer.get()))
    {
        ASR_THROW(get_last_error(), "win", "GetFileVersionInfoExW failed for ntdll");
    }

    VS_FIXEDFILEINFO* info {};
    UINT              size {};

    if (!::VerQueryValueW(
          buffer.get()
        , LR"(\)"
        , reinterpret_cast<void**>(&info)
        , &size))
    {
        ASR_THROW(get_last_error(), "win", "VerQueryValueW failed for ntdll");
    }

    auto const value =
          uint64_t {info->dwProductVersionMS} << 32u
        | uint64_t {info->dwProductVersionLS};

    return windows_version {value};
}

} // namespace

ASR_NOINLINE windows_version win_version() noexcept
{
    static windows_version const version = get_win_version();
    return version;
}

process_flags get_process_flags() noexcept
{
    auto const peb            = ::NtCurrentTeb()->ProcessEnvironmentBlock;
    auto const nt_global_flag = reinterpret_cast<char const*>(peb) + 0xbc;

    process_flags flags {};
    memcpy(&flags, nt_global_flag, sizeof(flags));

    return flags;
}

} // namespace asr::win
