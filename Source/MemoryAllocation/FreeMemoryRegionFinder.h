#pragma once

#include <cstddef>
#include <cstdint>

#include "FreeMemoryRegion.h"

struct FreeMemoryRegionFinder {
    explicit FreeMemoryRegionFinder(FreeMemoryRegion** firstFreeRegion) noexcept
        : firstFreeRegion{ firstFreeRegion }
    {
    }

    [[nodiscard]] FreeMemoryRegion** findFreeRegion(std::size_t size) const noexcept
    {
        for (FreeMemoryRegion** region = firstFreeRegion; *region != nullptr; region = &(*region)->nextFreeRegion) {
            if ((*region)->isLargeEnoughForAllocation(size))
                return region;
        }
        return nullptr;
    }

    [[nodiscard]] FreeMemoryRegion** findNearestFreeRegion(std::uintptr_t address) const noexcept
    {
        FreeMemoryRegion** region = firstFreeRegion;
        while (*region != nullptr && (*region)->nextFreeRegion != nullptr && reinterpret_cast<std::uintptr_t>((*region)->nextFreeRegion) < address)
            region = &(*region)->nextFreeRegion;
        return region;
    }

private:
    FreeMemoryRegion** firstFreeRegion;
};
