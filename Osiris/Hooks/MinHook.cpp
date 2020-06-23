#include "MinHook.h"
#include "../MinHook/MinHook.h"

void MinHook::init(void* base) noexcept
{
    this->base = base;
}

void MinHook::hookAt(std::size_t index, void* fun) noexcept
{
    void* orig;
    MH_CreateHook((*reinterpret_cast<void***>(base))[index], fun, &orig);
    originals[index] = uintptr_t(orig);
}
