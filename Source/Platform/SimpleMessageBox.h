#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN32() || IS_WIN64()

#include "Windows/WindowsMessageBox.h"

using SimpleMessageBox = WindowsMessageBox;

#elif IS_LINUX()

#include "Linux/LinuxMessageBox.h"

using SimpleMessageBox = LinuxMessageBox;

#endif
