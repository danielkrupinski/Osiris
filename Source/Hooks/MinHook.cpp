#include "MinHook.h"
#include <MinHook/MinHook.h>
#include "../Helpers.h"

void MinHook::init(void* base) noexcept
{
    this->base = base;
}

std::uintptr_t MinHook::hookAt(std::size_t index, void* fun) noexcept
{
    void* orig;
    MH_CreateHook((*reinterpret_cast<void***>(base))[index], fun, &orig);
    return std::uintptr_t(orig);
}
