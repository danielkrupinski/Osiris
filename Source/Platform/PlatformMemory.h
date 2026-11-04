#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN64()

#include "Windows/WindowsMemory.h"
using PlatformMemory = WindowsMemory;

#elif IS_LINUX()

#include "Linux/LinuxMemory.h"
using PlatformMemory = LinuxMemory;

#endif
