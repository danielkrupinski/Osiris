#pragma once

class Input {
public:
    std::byte pad[12];
    bool isTrackIRAvailable;
    bool isMouseInitialized;
    bool isMouseActive;
    std::byte pad1[164];
    bool isCameraInThirdPerson;
    std::byte pad2;
    Vector cameraOffset;
};
