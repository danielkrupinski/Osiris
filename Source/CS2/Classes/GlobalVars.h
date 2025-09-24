#pragma once

#include <Utils/Pad.h>

namespace cs2
{

struct GlobalVars {
    PAD(48); // FIXME: get offset to curtime dynamically
    float curtime;
    using frametime = float;
};

}
