#pragma once

#include "IsPlatform.h"

#if IS_WIN32()

#define WIN32_ARGS(...) __VA_ARGS__
#define LINUX_ARGS(...)
#define WIN32_LINUX(win32, linux) win32
#define FASTCALL_THIS(thisptr) thisptr, void*

#elif IS_WIN64()

#define WIN32_ARGS(...)
#define LINUX_ARGS(...)
#define WIN32_LINUX(win32, linux) win32
#define FASTCALL_THIS(thisptr) thisptr

#elif IS_LINUX()

#define WIN32_ARGS(...)
#define LINUX_ARGS(...) __VA_ARGS__
#define WIN32_LINUX(win32, linux) linux
#define FASTCALL_THIS(thisptr) thisptr

#else

#error "Unsupported platform"

#endif
