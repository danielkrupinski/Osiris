#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>

#include <Utils/Align.h>

#include "FreeMemoryRegionList.h"
#include "MemoryAllocatorBase.h"

template <typename T>
struct MemoryAllocator {
    [[nodiscard]] static std::byte* allocate() noexcept
    {
        return MemoryAllocatorBase::allocate(memoryFor());
    }

    static void deallocate(T* memory) noexcept
    {
        MemoryAllocatorBase::deallocate(reinterpret_cast<std::byte*>(memory), memoryFor());
    }

private:
    [[nodiscard]] static auto memoryFor() noexcept
    {
        static_assert(alignof(T) <= FreeMemoryRegionList::minimumAlignment(), "Unsupported alignment");
        return (std::max)(utils::align<sizeof(T), FreeMemoryRegionList::minimumAlignment()>(), FreeMemoryRegionList::minimumAllocationSize());
    }
};

template <typename T>
struct MemoryAllocator<T[]> {
    [[nodiscard]] static std::byte* allocate(std::size_t numberOfElements) noexcept
    {
        return MemoryAllocatorBase::allocate(memoryFor(numberOfElements));
    }

    static void deallocate(T* memory, std::size_t numberOfElements) noexcept
    {
        MemoryAllocatorBase::deallocate(reinterpret_cast<std::byte*>(memory), memoryFor(numberOfElements));
    }

private:
    [[nodiscard]] static auto memoryFor(std::size_t numberOfElements) noexcept
    {
        static_assert(alignof(T) <= FreeMemoryRegionList::minimumAlignment(), "Unsupported alignment");
        assert(numberOfElements <= (std::numeric_limits<std::size_t>::max)() / sizeof(std::remove_extent_t<T>));
        return (std::max)(utils::align(numberOfElements * sizeof(std::remove_extent_t<T>), FreeMemoryRegionList::minimumAlignment()), FreeMemoryRegionList::minimumAllocationSize());
    }
};
