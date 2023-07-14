#pragma once

#include "Macros/PlatformSpecific.h"

namespace platform
{
    constexpr auto lengthOfTypeInfoPrecedingVmt = WIN32_LINUX(1, 2);
}
