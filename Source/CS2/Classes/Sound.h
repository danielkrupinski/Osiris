#pragma once

#include "CUtlFilenameSymbolTable.h"
#include "CUtlVector.h"
#include "Vector.h"

#include <Utils/Pad.h>

namespace cs2
{

struct CSfxTable {
    PAD(40); // TODO: get dynamically, was broken: 2024.05.23
    FileNameHandle_t fileNameHandle;
};

struct ChannelInfo1 {
    CSfxTable* sfx;
    PAD(48);
    int guid;
    PAD(4);
};

static_assert(sizeof(ChannelInfo1) == 64);

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
