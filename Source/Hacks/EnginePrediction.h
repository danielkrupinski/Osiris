#pragma once

#include "../Memory.h"

class ClientInterfaces;
struct UserCmd;

namespace EnginePrediction
{
    void run(const ClientInterfaces& clientInterfaces, const Memory& memory, UserCmd* cmd) noexcept;
    int getFlags() noexcept;
}
