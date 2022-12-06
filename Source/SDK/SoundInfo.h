#pragma once

#include "Pad.h"
#include <Platform/PlatformSpecific.h>

struct SoundInfo {
    PAD(WIN32_LINUX(40, 44))
    float volume;
    PAD(12)
    int	entityIndex;
    int channel;
    int pitch;
    int flags;
    int soundIndex;
};
