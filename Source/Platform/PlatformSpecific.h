#pragma once

#include "IsPlatform.h"

#if IS_WIN32()

#define LINUX_ARGS(...)
#define WIN32_LINUX(win32, linux) win32

#elif IS_LINUX()

#define LINUX_ARGS(...) __VA_ARGS__
#define WIN32_LINUX(win32, linux) linux

#else

#define LINUX_ARGS(...)
#define WIN32_LINUX(win32, linux)

#endif
