#pragma once

#include <Platform/PlatformSpecific.h>
#include <SDK/Pad.h>

namespace csgo::pod
{

struct SharedObject;

struct SharedObjectTypeCache {
    PAD(sizeof(void*))
    csgo::pod::SharedObject** objects;
    PAD(WIN32_LINUX(16, 24))
    int objectCount;
    PAD(WIN32_LINUX(4, 12))
    int classID; // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_constants.h#L39
};

}
