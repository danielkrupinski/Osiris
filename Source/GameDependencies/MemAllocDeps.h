#pragma once

#include <cstdint>

#include <CS2/Classes/IMemAlloc.h>
#include <GameDLLs/Tier0Dll.h>
#include <Platform/DynamicLibrary.h>
#include <Utils/FieldOffset.h>

struct MemAllocDeps {
    template <typename MemAllocPatterns>
    MemAllocDeps(Tier0Dll tier0Dll, const MemAllocPatterns& memAllocPatterns) noexcept
        : thisptr{tier0Dll.memAlloc()}
        , alloc{memAllocPatterns.allocOffset()}
    {
    }

    [[nodiscard]] static const MemAllocDeps& instance() noexcept;

    cs2::IMemAlloc** thisptr;
    FieldOffset<const void, cs2::IMemAlloc::Alloc*, std::int8_t> alloc;
};
