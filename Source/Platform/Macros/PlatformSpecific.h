#pragma once

#include "IsPlatform.h"

#if IS_WIN64()

#define WIN64_ONLY(...) __VA_ARGS__
#define LINUX_ONLY(...)
#define WIN32_LINUX(win32, linux) win32
#define FASTCALL_THIS(thisptr) thisptr

#elif IS_LINUX()

#define WIN64_ONLY(...)
#define LINUX_ONLY(...) __VA_ARGS__
#define WIN32_LINUX(win32, linux) linux
#define FASTCALL_THIS(thisptr) thisptr

#else

#error "Unsupported platform"

#endif
