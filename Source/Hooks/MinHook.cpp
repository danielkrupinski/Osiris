#include "MinHook.h"
#include <MinHook/MinHook.h>
#include "../Helpers.h"

void MinHook::install(std::uintptr_t*& vmt) noexcept
{
    this->vmt = &vmt;
    installed = true;
}

std::uintptr_t MinHook::hook(std::size_t index, GenericFunctionPointer fun) noexcept
{
    void* orig;
    MH_CreateHook((void*)(*vmt)[index], (void*)static_cast<void(*)()>(fun), &orig);
    return std::uintptr_t(orig);
}
