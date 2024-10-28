#pragma once

#include <cstdint>

#include <CS2/Classes/IMemAlloc.h>
#include <GameDLLs/Tier0Dll.h>
#include <Platform/DynamicLibrary.h>
#include <Utils/FieldOffset.h>

struct MemAllocDeps {
    MemAllocDeps() = default;

    MemAllocDeps(Tier0Dll tier0Dll) noexcept
        : thisptr{tier0Dll.memAlloc()}
    {
    }

    cs2::IMemAlloc** thisptr;
};
