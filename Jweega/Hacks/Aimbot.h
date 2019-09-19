#pragma once

#include "../SDK/UserCmd.h"
#include <vector>

namespace Aimbot {
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(UserCmd*) noexcept;

    struct Enemies {
        float distance;
        int id;

        bool operator<(const Enemies& enemy) const noexcept {
            return distance < enemy.distance;
        }
    };
}
