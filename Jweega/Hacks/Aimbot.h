#pragma once

#include "../SDK/UserCmd.h"

namespace Aimbot {
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(UserCmd*) noexcept;

    struct Enemies {
        int health;
        float distance;
        int id;

        bool operator<(const Enemies& enemy) const noexcept {
            if (health != enemy.health)
                return health < enemy.health;
            return distance < enemy.distance;
        }
    };
}
