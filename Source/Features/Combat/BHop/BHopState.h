#pragma once

#include <cstdint>

struct BHopState {
    bool wasOnGround{false};
    bool shouldJump{false};
    bool autoStrafeActive{false};
    float strafeTarget{0.0f};
    float injectedSideButton{0.0f};
    float autoStrafeYawRemaining{0.0f};
    float lastStrafeTime{0.0f};
    // Humanization: randomly skip some jumps to break the perfect pattern
    std::uint8_t skipCounter{0};
    std::uint8_t skipThreshold{0};
};
