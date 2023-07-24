#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

struct FreeMemoryRegion {
    std::size_t size;
    FreeMemoryRegion* nextFreeRegion;

    [[nodiscard]] bool isLargeEnoughForAllocation(std::size_t allocationSize) const noexcept
    {
        if (size < allocationSize)
            return false;

        const auto sizeAfterAllocation = size - allocationSize;
        return sizeAfterAllocation == 0 || sizeAfterAllocation >= sizeof(FreeMemoryRegion);
    }

    void tryMergeWithNextRegion() noexcept
    {
        assert(nextFreeRegion == nullptr || reinterpret_cast<std::uintptr_t>(nextFreeRegion) > reinterpret_cast<std::uintptr_t>(this));
        if (nextFreeRegion && reinterpret_cast<std::uintptr_t>(nextFreeRegion) - reinterpret_cast<std::uintptr_t>(this) == size) {
            size += nextFreeRegion->size;
            nextFreeRegion = nextFreeRegion->nextFreeRegion;
        }
    }

    void appendRegion(std::byte* newRegionPointer, std::size_t newRegionSize) noexcept
    {
        assert(newRegionPointer != nullptr);
        assert(reinterpret_cast<std::uintptr_t>(newRegionPointer) > reinterpret_cast<std::uintptr_t>(this));
        assert(newRegionSize >= sizeof(FreeMemoryRegion));

        nextFreeRegion = new (newRegionPointer) FreeMemoryRegion{ newRegionSize, nextFreeRegion };
        nextFreeRegion->tryMergeWithNextRegion();
        tryMergeWithNextRegion();
    }
};
