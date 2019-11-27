#define WIN32_LEAN_AND_MEAN
#define STRICT
#define NOMINMAX

#define _CONTAINER_DEBUG_LEVEL 0
#define _ITERATOR_DEBUG_LEVEL  0

#include <sdkddkver.h>
#include <Windows.h>

extern "C" int __cdecl _except_handler3()
{
    __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    return 0;
}

extern "C" int __cdecl _except_handler4()
{
    __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    return 0;
}
