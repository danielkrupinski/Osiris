#pragma once

#include <cstdint>

#include <CS2/Classes/IMemAlloc.h>
#include <GameDLLs/Tier0Dll.h>
#include <MemoryPatterns/MemAllocPatterns.h>
#include <Platform/DynamicLibrary.h>
#include <Utils/FieldOffset.h>

struct MemAllocImpl {
    explicit MemAllocImpl(Tier0Dll tier0Dll) noexcept
        : thisptr{ tier0Dll.memAlloc() }
    {
    }

    [[nodiscard]] static const MemAllocImpl& instance() noexcept;

    cs2::IMemAlloc** thisptr;
    FieldOffset<const void, cs2::IMemAlloc::Alloc*, std::int8_t> alloc{ MemAllocPatterns::allocOffset() };
};
