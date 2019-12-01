#pragma once

#include <cstdint>

struct _EXCEPTION_POINTERS;

namespace asr::win
{

enum class module_version : intptr_t
{
      v1      = 1
    , current = v1
};

struct main_module_interface
{
    using main_t        = int (__cdecl*)(wchar_t const* command_line) noexcept;
    using fatal_error_t = void (__cdecl*)(_EXCEPTION_POINTERS*) noexcept; // [[noreturn]]

    module_version version;     //!< required
    main_t         dll_main;    //!< required
    fatal_error_t  fatal_error; //!< optional
};

inline constexpr char const* load_main_module = "init";

//! on invocation, out->version contains the verion of the host
using load_main_module_t = int (__cdecl*)(
    main_module_interface* out) noexcept;

} // namespace asr::win
