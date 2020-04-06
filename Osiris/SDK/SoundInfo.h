#pragma once

#include "Pad.h"

struct SoundInfo {
    PAD(40)
    float volume;
    PAD(12)
    int	entityIndex;
    PAD(0xC)
    int soundIndex;
};
