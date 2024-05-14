#pragma once

#include "IsPlatform.h"

#if IS_WIN64()

#define WIN64_ONLY(...) __VA_ARGS__
#define LINUX_ONLY(...)
#define WIN64_LINUX(win64, linux) win64

#elif IS_LINUX()

#define WIN64_ONLY(...)
#define LINUX_ONLY(...) __VA_ARGS__
#define WIN64_LINUX(win64, linux) linux

#else

#error "Unsupported platform"

#endif
