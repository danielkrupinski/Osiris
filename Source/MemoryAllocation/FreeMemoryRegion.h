#pragma once

#include <cstddef>
#include <functional>
#include <new>
#include <span>

class FreeMemoryRegion {
public:
    FreeMemoryRegion(const FreeMemoryRegion&) = delete;
    FreeMemoryRegion(FreeMemoryRegion&&) = delete;
    FreeMemoryRegion& operator=(const FreeMemoryRegion&) = delete;
    FreeMemoryRegion& operator=(FreeMemoryRegion&&) = delete;

    [[nodiscard]] static FreeMemoryRegion* create(std::span<std::byte> region) noexcept
    {
        return new (region.data()) FreeMemoryRegion{ region.size(), nullptr };
    }

    [[nodiscard]] std::size_t getSize() const noexcept
    {
        return size;
    }

    [[nodiscard]] FreeMemoryRegion* getNextFreeRegion() const noexcept
    {
        return nextFreeRegion;
    }

    [[nodiscard]] FreeMemoryRegion* addFreeRegion(std::span<std::byte> region) noexcept
    {
        if (shouldPrepend(region.data()))
            return prependRegion(region);
        findRegionToAppendTo(region.data()).appendRegion(region);
        return this;
    }

    struct MemoryClaimResult {
        FreeMemoryRegion* claimedRegion{nullptr};
        FreeMemoryRegion* regionToReplaceClaimed{nullptr};
    };

    [[nodiscard]] MemoryClaimResult claimMemory(std::size_t claimSize) noexcept
    {
        if (claimMemoryFromSelf(claimSize))
            return MemoryClaimResult{ .claimedRegion = this, .regionToReplaceClaimed = nextFreeRegion };

        for (FreeMemoryRegion* region = this; region->nextFreeRegion != nullptr; region = region->nextFreeRegion) {
            if (region->nextFreeRegion->claimMemoryFromSelf(claimSize)) {
                const auto claimedRegion = region->nextFreeRegion;
                region->nextFreeRegion = claimedRegion->nextFreeRegion;
                return MemoryClaimResult{ .claimedRegion = claimedRegion, .regionToReplaceClaimed = region->nextFreeRegion };
            }
        }

        return {};
    }

private:
    [[nodiscard]] bool claimMemoryFromSelf(std::size_t allocationSize) noexcept
    {
        if (size == allocationSize) {
           return true;
        } else if (size > allocationSize && size - allocationSize >= sizeof(FreeMemoryRegion)) {
            nextFreeRegion = new (reinterpret_cast<std::byte*>(this) + allocationSize) FreeMemoryRegion{ size - allocationSize, nextFreeRegion };
            return true;
        }
        return false;
    }

    [[nodiscard]] FreeMemoryRegion* prependRegion(std::span<std::byte> region) noexcept
    {
        if (region.data() + region.size() == reinterpret_cast<std::byte*>(this))
            return new (region.data()) FreeMemoryRegion{ region.size() + size, nextFreeRegion };
        else
            return new (region.data()) FreeMemoryRegion{ region.size(), this };
    }

    void appendRegion(std::span<std::byte> region) noexcept
    {
        if (reinterpret_cast<std::byte*>(this) + size == region.data()) {
            size += region.size();
            tryMergeWithNextRegion();
        } else {
            nextFreeRegion = new (region.data()) FreeMemoryRegion{ region.size(), nextFreeRegion};
            nextFreeRegion->tryMergeWithNextRegion();
        }
    }

    [[nodiscard]] FreeMemoryRegion& findRegionToAppendTo(std::byte* newRegionPointer) noexcept
    {
        FreeMemoryRegion* region = this;
        while (region != nullptr && region->getNextFreeRegion() != nullptr && std::less{}(reinterpret_cast<std::byte*>(region->getNextFreeRegion()), newRegionPointer))
            region = region->getNextFreeRegion();
        return *region;
    }

    [[nodiscard]] bool shouldPrepend(const std::byte* newRegionPointer) const noexcept
    {
        return std::less{}(newRegionPointer, reinterpret_cast<const std::byte*>(this));
    }

    void tryMergeWithNextRegion() noexcept
    {
        if (reinterpret_cast<std::byte*>(this) + size == reinterpret_cast<std::byte*>(nextFreeRegion)) {
            size += nextFreeRegion->size;
            nextFreeRegion = nextFreeRegion->nextFreeRegion;
        }
    }

    FreeMemoryRegion(std::size_t size, FreeMemoryRegion* nextFreeRegion) noexcept
        : size{ size }, nextFreeRegion{ nextFreeRegion }
    {
    }

    std::size_t size;
    FreeMemoryRegion* nextFreeRegion;
};
