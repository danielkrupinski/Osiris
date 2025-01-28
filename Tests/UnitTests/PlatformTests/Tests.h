#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN64()

#include "Windows/PebLdrTests.h"

#elif IS_LINUX()

#include "Linux/SharedObjectTests.h"

#endif
