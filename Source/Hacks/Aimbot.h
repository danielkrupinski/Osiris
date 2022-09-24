#pragma once

#include <Config.h>
#include "../Memory.h"

struct UserCmd;
struct Vector;

namespace Aimbot
{
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(const Interfaces& interfaces, const Config& config, const Memory& memory, UserCmd*) noexcept;

    void updateInput(const Config& config) noexcept;
}
