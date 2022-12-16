#pragma once

#include "../Memory.h"

class ClientInterfaces;
namespace csgo { struct UserCmd; }

namespace EnginePrediction
{
    void run(const ClientInterfaces& clientInterfaces, const Memory& memory, csgo::UserCmd* cmd) noexcept;
    int getFlags() noexcept;
}
