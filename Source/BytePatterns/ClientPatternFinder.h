#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32() || IS_WIN64()

#include "WindowsClientPatternFinder.h"
using ClientPatternFinder = WindowsClientPatternFinder;

#elif IS_LINUX()

#include "LinuxClientPatternFinder.h"
using ClientPatternFinder = LinuxClientPatternFinder;

#endif
