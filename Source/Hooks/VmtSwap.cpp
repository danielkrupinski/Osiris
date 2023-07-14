#include <cstdint>

#include "VmtSwap.h"

void VmtSwap::init(void* base) noexcept
{
    this->base = base;
    const auto vmt = *reinterpret_cast<std::uintptr_t**>(base);
    vmtCopy.emplace(vmt, vmtLengthCalculator(vmt));
    *reinterpret_cast<std::uintptr_t**>(base) = vmtCopy->getReplacementVmt();
}
