#pragma once

#include <Config.h>
#include "../Memory.h"

struct UserCmd;

namespace Triggerbot
{
    void run(const EngineTrace& engineTrace, const OtherInterfaces& interfaces, const Memory& memory, const Config& config, UserCmd*) noexcept;
    void updateInput(const Config& config) noexcept;
}
