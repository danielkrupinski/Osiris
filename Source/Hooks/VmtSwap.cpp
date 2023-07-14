#include <algorithm>

#include "VmtSwap.h"
#include "../Helpers.h"

void VmtSwap::init(void* base) noexcept
{
    this->base = base;
    oldVmt = *reinterpret_cast<std::uintptr_t**>(base);
    length = static_cast<std::size_t>(vmtLengthCalculator(oldVmt)) + platform::lengthOfTypeInfoPrecedingVmt;
    newVmt = std::make_unique<std::uintptr_t[]>(length);
    std::copy(oldVmt - platform::lengthOfTypeInfoPrecedingVmt, oldVmt - platform::lengthOfTypeInfoPrecedingVmt + length, newVmt.get());
    *reinterpret_cast<std::uintptr_t**>(base) = newVmt.get() + platform::lengthOfTypeInfoPrecedingVmt;
}
