#pragma once

#include <cstddef>
#include <type_traits>

#include <Utils/Align.h>

#include "FreeMemoryRegionList.h"

struct MemoryAllocator {
    template <typename T>
        requires (!std::is_array_v<T>)
    [[nodiscard]] static std::byte* allocate() noexcept
    {
        return allocate(memoryFor<T>());
    }

    template <typename T>
        requires std::is_unbounded_array_v<T>
    [[nodiscard]] static std::byte* allocate(std::size_t numberOfElements) noexcept
    {
        return allocate(memoryFor<T>(numberOfElements));
    }

    template <typename T, typename... Args>
        requires std::is_bounded_array_v<T>
    static void allocate(Args&&...) = delete;

    template <typename T>
        requires (!std::is_array_v<T>)
    static void deallocate(T* memory) noexcept
    {
        deallocate(reinterpret_cast<std::byte*>(memory), memoryFor<T>());
    }

    template <typename T>
        requires std::is_unbounded_array_v<T>
    static void deallocate(std::remove_extent_t<T>* memory, std::size_t numberOfElements) noexcept
    {
        deallocate(reinterpret_cast<std::byte*>(memory), memoryFor<T>(numberOfElements));
    }

    template <typename T, typename... Args>
        requires std::is_bounded_array_v<T>
    static void deallocate(Args&&...) = delete;

private:
    template <typename T>
        requires (!std::is_array_v<T>)
    [[nodiscard]] static auto memoryFor() noexcept
    {
        static_assert(alignof(T) <= FreeMemoryRegionList::minimumAlignment(), "Unsupported alignment");
        return utils::align<sizeof(T), FreeMemoryRegionList::minimumAlignment()>();
    }

    template <typename T>
        requires std::is_unbounded_array_v<T>
    [[nodiscard]] static auto memoryFor(std::size_t numberOfElements) noexcept
    {
        static_assert(alignof(T) <= FreeMemoryRegionList::minimumAlignment(), "Unsupported alignment");
        assert(numberOfElements <= (std::numeric_limits<std::size_t>::max)() / sizeof(std::remove_extent_t<T>));
        return utils::align(numberOfElements * sizeof(std::remove_extent_t<T>), FreeMemoryRegionList::minimumAlignment());
    }

    [[nodiscard]] static std::byte* allocate(std::size_t size) noexcept;
    static void deallocate(std::byte* memory, std::size_t size) noexcept;
};
