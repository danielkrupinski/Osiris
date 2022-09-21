#pragma once

#include <Config.h>

struct UserCmd;

namespace Triggerbot
{
    void run(const Config& config, UserCmd*) noexcept;
    void updateInput(const Config& config) noexcept;
}
