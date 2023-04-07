#pragma once

#include <cstddef>

#include "VirtualMethod.h"

namespace csgo
{

struct MemAllocPOD;

struct MemAlloc : GameClass<MemAlloc, MemAllocPOD> {
    VIRTUAL_METHOD(void*, alloc, 1, (std::size_t size), (size))

    [[nodiscard]] void* allocAligned(std::size_t size, std::size_t alignment) const noexcept
    {
        const auto padding = alignment - 1 + sizeof(void*);
        const auto memory = alloc(size + padding);
        const auto alignedMemory = reinterpret_cast<void*>((std::uintptr_t(memory) + padding) & ~(alignment - 1));
        reinterpret_cast<void**>(alignedMemory)[-1] = memory;
        return alignedMemory;
    }
};

}
