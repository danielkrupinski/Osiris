#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN32()
#include "Windows/DynamicLibraryWrapper.h"
using DynamicLibraryWrapper = windows_platform::DynamicLibraryWrapper;
#elif IS_LINUX()
#include "Linux/DynamicLibraryWrapper.h"
using DynamicLibraryWrapper = linux_platform::DynamicLibraryWrapper;
#endif
