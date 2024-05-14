#pragma once

#include "Macros/PlatformSpecific.h"

namespace platform
{
    constexpr auto lengthOfTypeInfoPrecedingVmt = WIN64_LINUX(1, 2);
}
