#pragma once

#include "../Interfaces.h"
#include "../Memory.h"

struct UserCmd;

namespace EnginePrediction
{
    void run(const Interfaces& interfaces, const Memory& memory, UserCmd* cmd) noexcept;
    int getFlags() noexcept;
}
