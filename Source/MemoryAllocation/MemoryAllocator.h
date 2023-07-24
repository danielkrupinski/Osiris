#pragma once

#include <cstddef>
#include <type_traits>

#include <Utils/Align.h>

#include "FreeMemoryRegionList.h"

struct MemoryAllocator {
    [[nodiscard]] static std::byte* allocate(std::size_t size) noexcept;
    static void deallocate(std::byte* memory, std::size_t size) noexcept;
    
    template <typename T>
    [[nodiscard]] static auto memoryFor() noexcept
    {
        static_assert(!std::is_array_v<T>);
        static_assert(alignof(T) <= FreeMemoryRegionList::minimumAlignment(), "Unsupported alignment");
        return utils::align<sizeof(T), FreeMemoryRegionList::minimumAlignment()>();
    }

    template <typename T>
    [[nodiscard]] static auto memoryFor(std::size_t numberOfElements) noexcept
    {
        static_assert(std::is_array_v<T>);
        static_assert(alignof(T) <= FreeMemoryRegionList::minimumAlignment(), "Unsupported alignment");
        assert(numberOfElements <= (std::numeric_limits<std::size_t>::max)() / sizeof(std::remove_extent_t<T>));
        return utils::align(numberOfElements * sizeof(std::remove_extent_t<T>), FreeMemoryRegionList::minimumAlignment());
    }
};
