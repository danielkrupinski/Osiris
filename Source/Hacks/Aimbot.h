#pragma once

#include <Config.h>

struct UserCmd;
struct Vector;

namespace Aimbot
{
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(const Config& config, UserCmd*) noexcept;

    void updateInput(const Config& config) noexcept;
}
