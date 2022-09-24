#pragma once

#include <Config.h>
#include "../Interfaces.h"
#include "../Memory.h"

struct UserCmd;

namespace Triggerbot
{
    void run(const Interfaces& interfaces, const Memory& memory, const Config& config, UserCmd*) noexcept;
    void updateInput(const Config& config) noexcept;
}
