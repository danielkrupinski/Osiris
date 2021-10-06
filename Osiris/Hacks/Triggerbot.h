#pragma once

struct UserCmd;

namespace Triggerbot
{
    void run(UserCmd*) noexcept;
    void updateInput() noexcept;
}
