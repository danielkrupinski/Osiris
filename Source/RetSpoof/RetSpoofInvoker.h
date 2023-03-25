#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include "RetSpoofInvokerWindows.h"
using RetSpoofInvoker = RetSpoofInvokerWindows;
#elif IS_WIN64()
#include "RetSpoofInvokerLinux.h"
using RetSpoofInvoker = RetSpoofInvokerLinux;
#elif IS_LINUX()
#include "RetSpoofInvokerLinux.h"
using RetSpoofInvoker = RetSpoofInvokerLinux;
#endif
