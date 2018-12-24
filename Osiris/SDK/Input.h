#pragma once

class Input {
public:
    std::byte pad[12];
    bool isTrackIRAvailable;
    bool isMouseInitialized;
    bool isMouseActive;
};
