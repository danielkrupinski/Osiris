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
    int guid;
    PAD(60); // TODO: get sizeof dynamically, was broken: 2024.02.07, broken again (reverted to previous) 2024.05.23
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
