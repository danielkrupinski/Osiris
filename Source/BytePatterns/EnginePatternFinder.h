#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32() || IS_WIN64()

#include "WindowsEnginePatternFinder.h"
using EnginePatternFinder = WindowsEnginePatternFinder;

#elif IS_LINUX()

#include "LinuxEnginePatternFinder.h"
using EnginePatternFinder = LinuxEnginePatternFinder;

#endif
