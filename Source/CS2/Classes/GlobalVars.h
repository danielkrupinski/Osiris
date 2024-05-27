#pragma once

#include <Utils/Pad.h>

namespace cs2
{

struct GlobalVars {
    PAD(52); // FIXME: get offset to curtime dynamically
    float curtime;
};

}
