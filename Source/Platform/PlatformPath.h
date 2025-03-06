#pragma once

#include "Macros/PlatformSpecific.h"

namespace platform
{

using PathCharType = WIN64_LINUX(wchar_t, char);

constexpr PathCharType kPathSeparator = WIN64_LINUX(L'\\', '/');

}
