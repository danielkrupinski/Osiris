#pragma once

#include <cstddef>

struct AnimState {
    std::byte pad[164];
    float duckAmount;
    std::byte pad1[80];
    float footSpeed;
    float footSpeed2;
    std::byte pad2[22];
    float stopToFullRunningFraction;
    std::byte pad3[532];
    float velocitySubtractY;
};
