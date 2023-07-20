#pragma once

#include <cstddef>

struct MemoryAllocator {
    [[nodiscard]] static std::byte* allocate(std::size_t size) noexcept;
    static void deallocate(std::byte* memory, std::size_t size) noexcept;
};
