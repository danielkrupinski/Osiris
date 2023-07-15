#include <cstdint>

#include "VmtSwap.h"

void VmtSwap::init(void* base) noexcept
{
    this->base = base;
    swapper.install(*reinterpret_cast<std::uintptr_t**>(base));
}
