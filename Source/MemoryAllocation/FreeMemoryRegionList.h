#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>

#include "FreeMemoryRegion.h"
#include "FreeMemoryRegionFinder.h"

#ifndef NDEBUG
#include <Utils/MemorySection.h>
#endif

class FreeMemoryRegionList {
public:
    explicit FreeMemoryRegionList(std::span<std::byte> memory) noexcept
        : firstFreeRegion{ createInitialFreeRegion(memory) }
#ifndef NDEBUG
        , memory{ memory }
#endif
    {
    }

    FreeMemoryRegionList(const FreeMemoryRegionList&) = delete;
    FreeMemoryRegionList(FreeMemoryRegionList&&) = delete;
    FreeMemoryRegionList& operator=(const FreeMemoryRegionList&) = delete;
    FreeMemoryRegionList& operator=(FreeMemoryRegionList&&) = delete;

    [[nodiscard]] static constexpr std::size_t minimumAlignment() noexcept
    {
        return alignof(FreeMemoryRegion);
    }

    [[nodiscard]] static constexpr std::size_t minimumAllocationSize() noexcept
    {
        return sizeof(FreeMemoryRegion);
    }

#ifndef NDEBUG
    ~FreeMemoryRegionList() noexcept
    {
        assert(firstFreeRegion != nullptr && firstFreeRegion->size == memory.size() && firstFreeRegion->nextFreeRegion == nullptr && "Memory leak detected");
    }
#endif

    [[nodiscard]] std::byte* allocate(std::size_t size) noexcept
    {
        assert(size >= minimumAllocationSize());
        assert(size % minimumAlignment() == 0);

        const auto freeRegion = FreeMemoryRegionFinder{ &firstFreeRegion }.findFreeRegion(size);
        if (freeRegion == nullptr) [[unlikely]]
            return nullptr;

        const auto result = reinterpret_cast<std::byte*>(*freeRegion);
        if ((*freeRegion)->size != size)
            *freeRegion = new (result + size) FreeMemoryRegion{ (*freeRegion)->size - size, (*freeRegion)->nextFreeRegion };
        else
            *freeRegion = (*freeRegion)->nextFreeRegion;
        return result;
    }

    void deallocate(std::byte* pointer, std::size_t size) noexcept
    {
        assert(MemorySection{ memory }.contains(reinterpret_cast<std::uintptr_t>(pointer), size) && "Invalid pointer or size provided for deallocation");

        const auto nearestRegion = FreeMemoryRegionFinder{ &firstFreeRegion }.findNearestFreeRegion(reinterpret_cast<std::uintptr_t>(pointer));
        const auto noFreeRegionsBeforeDeallocation = (*nearestRegion == nullptr);
        if (noFreeRegionsBeforeDeallocation || reinterpret_cast<std::uintptr_t>(*nearestRegion) > reinterpret_cast<std::uintptr_t>(pointer)) {
            *nearestRegion = new (pointer) FreeMemoryRegion{ size, *nearestRegion };
        } else {
            (*nearestRegion)->appendRegion(pointer, size);
        }

        if (!noFreeRegionsBeforeDeallocation)
            (*nearestRegion)->tryMergeWithNextRegion();
    }

    [[nodiscard]] std::size_t getFreeSpace() const noexcept
    {
        std::size_t freeSpace = 0;
        for (const FreeMemoryRegion* region = firstFreeRegion; region != nullptr; region = region->nextFreeRegion)
            freeSpace += region->size;
        return freeSpace;
    }

private:
    [[nodiscard]] static FreeMemoryRegion* createInitialFreeRegion(std::span<std::byte> memory) noexcept
    {
        assert(memory.size() >= minimumAllocationSize());
        assert(std::uintptr_t(memory.data()) % minimumAlignment() == 0);
        return new (memory.data()) FreeMemoryRegion{ memory.size(), nullptr };
    }

    FreeMemoryRegion* firstFreeRegion;
#ifndef NDEBUG
    std::span<std::byte> memory;
#endif
};
