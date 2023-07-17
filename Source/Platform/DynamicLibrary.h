#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN32() || IS_WIN64()

#include "Windows/WindowsDynamicLibrary.h"

using DynamicLibrary = WindowsDynamicLibrary;

#elif IS_LINUX()

#include "Linux/LinuxDynamicLibrary.h"

using DynamicLibrary = LinuxDynamicLibrary;

#endif
