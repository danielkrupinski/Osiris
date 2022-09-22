#pragma once

#include "Memory.h"

namespace EventListener
{
    void init(const Memory& memory) noexcept;
    void remove(const Memory& memory) noexcept;
}
