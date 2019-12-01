#pragma once

#include "win_common.h"

#include <system_error>

namespace asr::win
{

enum class windows_error : DWORD;
enum class com_error     : HRESULT;

[[nodiscard]] std::error_category const& windows_error_category() noexcept;

[[nodiscard]] inline std::error_code make_error_code(windows_error const e) noexcept
{
    return std::error_code(static_cast<int>(e), windows_error_category());
}

[[nodiscard]] inline std::error_code make_error_code(com_error const e) noexcept
{
    return std::error_code(static_cast<int>(e), windows_error_category());
}

[[nodiscard]] inline std::error_code make_windows_error(DWORD const e) noexcept
{
    return make_error_code(static_cast<windows_error>(e));
}

[[nodiscard]] inline std::error_code make_com_error(HRESULT const e) noexcept
{
    return make_error_code(static_cast<windows_error>(e));
}

template <typename T>
void make_windows_error(T) = delete;

template <typename T>
void make_com_error(T) = delete;

[[nodiscard]] inline std::error_code get_last_error() noexcept
{
    return make_error_code(static_cast<windows_error>(::GetLastError()));
}

} // namespace asr::win
