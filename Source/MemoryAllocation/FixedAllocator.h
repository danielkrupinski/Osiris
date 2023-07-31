#pragma once

#include <cstddef>

#include "FreeMemoryRegionList.h"

template <std::size_t Capacity>
class FixedAllocator {
public:
    FixedAllocator() = default;
    FixedAllocator(const FixedAllocator&) = delete;
    FixedAllocator(FixedAllocator&&) = delete;
    FixedAllocator& operator=(const FixedAllocator&) = delete;
    FixedAllocator& operator=(FixedAllocator&&) = delete;

    static_assert(Capacity >= FreeMemoryRegionList::minimumAllocationSize());

    [[nodiscard]] std::byte* allocate(std::size_t size) noexcept
    {
        return freeRegionList.allocate(size);
    }

    void deallocate(std::byte* pointer, std::size_t size) noexcept
    {
        freeRegionList.deallocate(pointer, size);
    }

    [[nodiscard]] std::size_t getFreeSpace() const noexcept
    {
        return freeRegionList.getFreeSpace();
    }

private:
    alignas(FreeMemoryRegionList::minimumAlignment()) std::byte storage[Capacity];
    FreeMemoryRegionList freeRegionList{ storage };
};
