#pragma once

#include <Utils/Pad.h>

namespace cs2
{

struct GlobalVars {
    PAD(48); // FIXME: get offsets to frametime and curtime dynamically
    float frametime;
    float curtime;
};

}
