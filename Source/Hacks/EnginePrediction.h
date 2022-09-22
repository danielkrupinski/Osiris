#pragma once

struct UserCmd;

namespace EnginePrediction
{
    void run(const Memory& memory, UserCmd* cmd) noexcept;
    int getFlags() noexcept;
}
