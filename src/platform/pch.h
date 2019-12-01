#pragma once

#if !defined(ASR_NO_PCH)

#include "config.h"

#if defined(ASR_PLATFORM_WINDOWS)
#   include "windows/win_pch.h"
#else
#
#endif

#include <atomic>
#include <array>
#include <mutex>
#include <exception>
#include <memory>
#include <optional>
#include <string_view>
#include <array>
#include <string>
#include <utility>

#endif // #if !defined(ASR_NO_PCH)
