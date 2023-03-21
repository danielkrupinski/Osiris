#pragma once

#include <Utils/Pad.h>
#include <Platform/Macros/PlatformSpecific.h>

namespace csgo
{

struct SoundInfo {
    PAD(WIN32_LINUX(40, 44));
    float volume;
    PAD(12);
    int	entityIndex;
    int channel;
    int pitch;
    int flags;
    int soundIndex;
    PAD(16);
};

}
