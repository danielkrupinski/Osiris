#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>

#include "FreeMemoryRegion.h"

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
        assert(firstFreeRegion != nullptr && firstFreeRegion->getSize() == memory.size() && firstFreeRegion->getNextFreeRegion() == nullptr && "Memory leak detected");
    }
#endif

    [[nodiscard]] std::byte* allocate(std::size_t size) noexcept
    {
        assert(size >= minimumAllocationSize());
        assert(size % minimumAlignment() == 0);

        if (firstFreeRegion == nullptr) [[unlikely]]
            return nullptr;

        const auto [claimedRegion, regionToReplaceClaimed] = firstFreeRegion->claimMemory(size);
        if (claimedRegion == firstFreeRegion)
            firstFreeRegion = regionToReplaceClaimed;
        return reinterpret_cast<std::byte*>(claimedRegion);
    }

    void deallocate(std::byte* pointer, std::size_t size) noexcept
    {
        assert(MemorySection{ memory }.contains(reinterpret_cast<std::uintptr_t>(pointer), size) && "Invalid pointer or size provided for deallocation");
        firstFreeRegion = createOrAddRegion({ pointer, size });
    }

    [[nodiscard]] std::size_t getFreeSpace() const noexcept
    {
        std::size_t freeSpace = 0;
        for (const FreeMemoryRegion* region = firstFreeRegion; region != nullptr; region = region->getNextFreeRegion())
            freeSpace += region->getSize();
        return freeSpace;
    }

private:
    [[nodiscard]] FreeMemoryRegion* createOrAddRegion(std::span<std::byte> region) const noexcept
    {
        if (firstFreeRegion == nullptr) [[unlikely]] {
            return FreeMemoryRegion::create(region);
        } else {
            return firstFreeRegion->addFreeRegion(region);
        }
    }

    [[nodiscard]] static FreeMemoryRegion* createInitialFreeRegion(std::span<std::byte> memory) noexcept
    {
        assert(memory.size() >= minimumAllocationSize());
        assert(std::uintptr_t(memory.data()) % minimumAlignment() == 0);
        return FreeMemoryRegion::create(memory);
    }

    FreeMemoryRegion* firstFreeRegion;
#ifndef NDEBUG
    std::span<std::byte> memory;
#endif
};
