#pragma once

#include <cstddef>
#include <memory>

#include "MemoryAllocator.h"

template <typename T>
struct MemoryDeleter {
    void operator()(T* memory) const noexcept
    {
        if (memory) {
            std::destroy_at(memory);
            MemoryAllocator<T>::deallocate(memory);
        }
    }
};

template <typename T>
struct MemoryDeleter<T[]> {
    explicit MemoryDeleter(std::size_t numberOfElements) noexcept
        : numberOfElements{ numberOfElements }
    {
    }

    MemoryDeleter() = default;

    void operator()(T* memory) const noexcept
    {
        if (memory) {
            std::destroy_n(memory, numberOfElements);
            MemoryAllocator<T[]>::deallocate(memory, numberOfElements);
        }
    }

    [[nodiscard]] std::size_t getNumberOfElements() const noexcept
    {
        return numberOfElements;
    }

private:
    std::size_t numberOfElements = 0;
};
