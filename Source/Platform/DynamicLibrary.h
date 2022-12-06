#pragma once

#if IS_WIN32()
#include "Windows/DynamicLibrary.h"

template <typename DynamicLibraryWrapper>
using DynamicLibrary = windows_platform::DynamicLibrary<DynamicLibraryWrapper>;
#else
#include "Linux/SharedObject.h"

template <typename DynamicLibraryWrapper>
using DynamicLibrary = linux_platform::SharedObject<DynamicLibraryWrapper>;
#endif
