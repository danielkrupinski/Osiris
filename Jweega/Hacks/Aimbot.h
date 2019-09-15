#pragma once

#include "../SDK/UserCmd.h"
#include <vector>

namespace Aimbot {
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(UserCmd*) noexcept;

    struct Targets {
        float distance;
        float fov;
        Vector bonePosition;

        bool operator<(const Targets& targets) const noexcept {
            return distance < targets.distance;
        }
    };

    struct Enemies {
        int id;
        std::vector<Targets> targets;

        bool operator<(const Enemies& enemies) const noexcept {
            return targets < enemies.targets;
        }
    };
}
