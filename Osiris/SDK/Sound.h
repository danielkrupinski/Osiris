#pragma once

#include "Pad.h"
#include "Vector.h"

struct ActiveChannels {
    int count;
    short list[128];
};

struct Channel {
#ifdef _WIN32
    PAD(244)
#else
    PAD(260)
#endif
    int soundSource;
    PAD(56)
    Vector origin;
    Vector direction;
    PAD(80)
};
