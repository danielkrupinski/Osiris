#pragma once

#include <CS2/Classes/IMemAlloc.h>
#include <GameClient/DLLs/Tier0Dll.h>

struct MemAllocDeps {
    MemAllocDeps() = default;

    MemAllocDeps(Tier0Dll tier0Dll) noexcept
        : thisptr{tier0Dll.memAlloc()}
    {
    }

    cs2::IMemAlloc** thisptr;
};
