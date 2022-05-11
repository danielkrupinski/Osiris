#pragma once

#include "Pad.h"
#include "Vector.h"
#include "UserCmd.h"

class Input {
public:
    PAD(WIN32_LINUX(12, 16))
    bool isTrackIRAvailable;
    bool isMouseInitialized;
    bool isMouseActive;
    PAD(WIN32_LINUX(154, 162))
    bool isCameraInThirdPerson;
    bool cameraMovingWithMouse;
    Vector cameraOffset;
    bool cameraDistanceMove;
    int cameraOldX;
    int cameraOldY;
    int cameraX;
    int cameraY;
    bool cameraIsOrthographic;
    Vector previousViewAngles;
    Vector previousViewAnglesTilt;
    float lastForwardMove;
    int clearInputState;
    UserCmd* commands;
    VerifiedUserCmd* verifiedCommands;

    UserCmd* getUserCmd(int sequence_number)
    {
        return &commands[sequence_number % 150];
    }

    VerifiedUserCmd* getVerifiedUserCmd(int sequence_number)
    {
        return &verifiedCommands[sequence_number % 150];
    }
};
