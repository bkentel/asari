#pragma once

#include "config.h"

#include <compare>
#include <cstdint>

namespace asr::win
{

struct windows_version
{
    constexpr windows_version() = default;

    explicit constexpr windows_version(uint64_t const value) noexcept
        : value(value)
    {
    }

    constexpr windows_version(
          uint16_t const major
        , uint16_t const minor
        , uint16_t const sub_minor
        , uint16_t const build
    ) noexcept
        : value(uint64_t {major}     << (16u * 3u)
              | uint64_t {minor}     << (16u * 2u)
              | uint64_t {sub_minor} << (16u * 1u)
              | uint64_t {build}     << (16u * 0u))
    {
    }

    constexpr uint16_t major()     const noexcept { return static_cast<uint16_t>(value >> (16u * 3u) & 0xFFFFu); }
    constexpr uint16_t minor()     const noexcept { return static_cast<uint16_t>(value >> (16u * 2u) & 0xFFFFu); }
    constexpr uint16_t sub_minor() const noexcept { return static_cast<uint16_t>(value >> (16u * 1u) & 0xFFFFu); }
    constexpr uint16_t build()     const noexcept { return static_cast<uint16_t>(value >> (16u * 0u) & 0xFFFFu); }

    constexpr auto operator<=>(windows_version const& other) const noexcept = default;

    uint64_t value = 0;
};

inline constexpr windows_version win7   {6, 1, 7601, 0};
inline constexpr windows_version win8   {6, 2, 9200, 0};
inline constexpr windows_version win8_1 {6, 3, 9600, 0};

[[nodiscard]] ASR_NOINLINE windows_version win_version() noexcept;

enum class process_flags : uint32_t
{
    FLG_APPLICATION_VERIFIER = 0x100
};

[[nodiscard]] process_flags get_process_flags() noexcept;

[[nodiscard]] inline bool is_app_verifier_running() noexcept
{
    return !!(static_cast<uint32_t>(get_process_flags())
            & static_cast<uint32_t>(process_flags::FLG_APPLICATION_VERIFIER));
}

} // namespace asr::win
