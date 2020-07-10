#pragma once

#include "Pad.h"
#include "Vector.h"

struct ActiveChannels {
    int count;
    short list[128];
};

struct Channel {
    PAD(244)
    int soundSource;
    PAD(56)
    Vector origin;
    Vector direction;
    PAD(80)
};
