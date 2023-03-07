#pragma once

#include <Config.h>
#include "../Memory.h"

namespace csgo
{

struct UserCmd;
struct EngineTrace;

}

namespace Triggerbot
{
    void run(const csgo::EngineTrace& engineTrace, const OtherInterfaces& interfaces, const Memory& memory, const Config& config, csgo::UserCmd*) noexcept;
    void updateInput(const Config& config) noexcept;
}
