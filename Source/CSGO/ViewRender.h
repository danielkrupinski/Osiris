#pragma once

#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/Pad.h>

namespace csgo
{

struct ViewRender {
    PAD(WIN32_LINUX(0x588, 0x648));
    float smokeOverlayAmount;
};

}
