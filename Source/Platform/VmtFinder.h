#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN64()

#include "Windows/WindowsVmtFinder.h"

using VmtFinder = WindowsVmtFinder;

#elif IS_LINUX()

#include "Linux/LinuxVmtFinder.h"

using VmtFinder = LinuxVmtFinder;

#endif
