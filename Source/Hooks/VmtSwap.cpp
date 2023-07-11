#include <algorithm>

#include "VmtSwap.h"
#include "../Helpers.h"

void VmtSwap::init(void* base) noexcept
{
    this->base = base;
    oldVmt = *reinterpret_cast<std::uintptr_t**>(base);
    length = static_cast<std::size_t>(vmtLengthCalculator(oldVmt)) + dynamicCastInfoLength;
    newVmt = std::make_unique<std::uintptr_t[]>(length);
    std::copy(oldVmt - dynamicCastInfoLength, oldVmt - dynamicCastInfoLength + length, newVmt.get());
    *reinterpret_cast<std::uintptr_t**>(base) = newVmt.get() + dynamicCastInfoLength;
}
