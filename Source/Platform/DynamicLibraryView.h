#pragma once

#ifdef _WIN32
#include "Windows/DynamicLibraryView.h"

template <typename DynamicLibraryWrapper>
using DynamicLibraryView = windows_platform::DynamicLibraryView<DynamicLibraryWrapper>;
#else
#include "Linux/DynamicLibraryView.h"

template <typename DynamicLibraryWrapper>
using DynamicLibraryView = linux_platform::DynamicLibraryView<DynamicLibraryWrapper>;
#endif
