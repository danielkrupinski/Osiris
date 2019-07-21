#pragma once

#include "../SDK/UserCmd.h"

namespace Aimbot {
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    auto calculateDistance(const Vector& localPlayer, const Vector& Entity) noexcept;
    auto calculateAngleDifference(const Vector& viewAngles, const Vector& destination, float distance);
    void run(UserCmd*) noexcept;
}
