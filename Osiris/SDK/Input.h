#pragma once

#include "Vector.h"

class Input {
public:
    std::byte pad[12];
    bool isTrackIRAvailable;
    bool isMouseInitialized;
    bool isMouseActive;
    std::byte pad1[158];
    bool isCameraInThirdPerson;
    std::byte pad2;
    Vector cameraOffset;

	UserCmd* GetUserCmd(int nSlot, int sequence_number) { 
		return VirtualMethod::call<UserCmd*, 8, int, int>(this, nSlot, sequence_number);
	}
};
