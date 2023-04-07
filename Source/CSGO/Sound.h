#pragma once

#include <Utils/Pad.h>
#include "Vector.h"

namespace csgo
{

struct ActiveChannels {
    int count;
    short list[128];
};

struct Channel {
#if IS_WIN32()
    PAD(244);
#else
    PAD(260);
#endif
    int soundSource;
    PAD(56);
    Vector origin;
    Vector direction;
    PAD(80);
};

}
