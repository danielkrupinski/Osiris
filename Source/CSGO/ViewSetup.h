#pragma once

#include <Utils/Pad.h>

namespace csgo
{

struct ViewSetup {
    PAD(172);
    void* csm;
    float fov;
    PAD(32);
    float farZ;
};

}
