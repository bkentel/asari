#pragma once

#if defined(_MSC_VER) && defined(_MSC_FULL_VER) && defined(_MSC_BUILD)
#   define ASR_COMPILER_MSVC
#else
#   define ASR_COMPILER_CLANG
#endif

#if defined(_WIN32) || defined(_WIN64)
#   define ASR_PLATFORM_WINDOWS
#else
#   define ASR_PLATFORM_LINUX
#endif

#if defined(ASR_COMPILER_MSVC)
#   define ASR_NOINLINE __declspec(noinline)
#else
#   define ASR_NOINLINE
#endif

#define ASR_APPEND_IMPL(x, y) x ## y

#define ASR_APPEND(x, y) ASR_APPEND_IMPL(x, y)
