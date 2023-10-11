#pragma once

#include <cstdint>

#include <CS2/Classes/IMemAlloc.h>
#include <CS2/Constants/DllNames.h>
#include <MemoryPatterns/MemAllocPatterns.h>
#include <Platform/DynamicLibrary.h>
#include <Utils/FieldOffset.h>

struct MemAllocImpl {
    [[nodiscard]] static const MemAllocImpl& instance() noexcept;

    cs2::IMemAlloc** thisptr{ DynamicLibrary{cs2::TIER0_DLL}.getFunctionAddress("g_pMemAlloc").as<cs2::IMemAlloc**>() };
    FieldOffset<const void, cs2::IMemAlloc::Alloc*, std::int8_t> alloc{ MemAllocPatterns::allocOffset() };
};
