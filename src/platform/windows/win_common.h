#pragma once

#if !defined(STRICT)
#   define STRICT
#endif

#if !defined(NOMINMAX)
#   define NOMINMAX
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#   define WIN32_LEAN_AND_MEAN
#endif

#include <sdkddkver.h>
#include <Windows.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;
