#pragma once

#include <Utils/Pad.h>
#include "Vector.h"

namespace csgo
{

struct Input {
    PAD(WIN32_LINUX(12, 16));
    bool isTrackIRAvailable;
    bool isMouseInitialized;
    bool isMouseActive;
    PAD(WIN32_LINUX(154, 162));
    bool isCameraInThirdPerson;
    bool cameraMovingWithMouse;
    Vector cameraOffset;
};

}
