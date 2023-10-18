#pragma once

#include "CUtlFilenameSymbolTable.h"
#include "CUtlVector.h"
#include "Vector.h"

#include <Utils/Pad.h>

namespace cs2
{

struct CSfxTable {
    PAD(48);
    FileNameHandle_t fileNameHandle;
};

struct ChannelInfo1 {
    CSfxTable* sfx;
    int guid;
    PAD(60);
};

static_assert(sizeof(ChannelInfo1) == 72);

struct ChannelInfo2 {
    Vector origin;
    PAD(120);
};

static_assert(sizeof(ChannelInfo2) == 132);

struct SoundChannels {
    CUtlVector<ChannelInfo1> channelInfo1;
    CUtlVector<ChannelInfo2> channelInfo2;
};

}
