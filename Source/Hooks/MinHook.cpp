#include "MinHook.h"
#include <MinHook/MinHook.h>
#include "../Helpers.h"

void MinHook::install(std::uintptr_t*& vmt) noexcept
{
    this->vmt = &vmt;
    installed = true;
}

std::uintptr_t MinHook::hook(std::size_t index, std::uintptr_t fun) noexcept
{
    void* orig;
    MH_CreateHook((void*)(*vmt)[index], (void*)fun, &orig);
    return std::uintptr_t(orig);
}
