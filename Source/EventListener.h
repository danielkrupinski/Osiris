#pragma once

#include "Interfaces.h"
#include "Memory.h"

namespace EventListener
{
    void init(const Interfaces& interfaces, const Memory& memory) noexcept;
    void remove(const Interfaces& interfaces, const Memory& memory) noexcept;
}
