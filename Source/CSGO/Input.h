#pragma once

#include "Pad.h"
#include "Vector.h"

namespace csgo
{

class Input {
public:
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
