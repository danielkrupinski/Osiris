#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>

#include <MemoryAllocation/MemoryAllocator.h>
#include <MemoryAllocation/MemoryDeleter.h>
#include <MemoryAllocation/UniquePtr.h>

template <typename T>
    requires std::is_trivially_copyable_v<T>
class DynamicArray {
public:
    DynamicArray() = default;
    DynamicArray(DynamicArray&&) = delete;
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(DynamicArray&&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    [[nodiscard]] T* begin() noexcept
    {
        return memory.get();
    }

    [[nodiscard]] T* end() noexcept
    {
        return memory.get() + size;
    }

    [[nodiscard]] const T* begin() const noexcept
    {
        return memory.get();
    }

    [[nodiscard]] const T* end() const noexcept
    {
        return memory.get() + size;
    }

    [[nodiscard]] std::size_t getCapacity() const noexcept
    {
        return memory.get_deleter().getNumberOfElements();
    }

    [[nodiscard]] std::size_t getSize() const noexcept
    {
        return size;
    }

    [[nodiscard]] T& operator[](std::size_t index) noexcept
    {
        return elementAt(index);
    }

    [[nodiscard]] const T& operator[](std::size_t index) const noexcept
    {
        return elementAt(index);
    }

    bool pushBack(const T& value) noexcept
    {
        if (ensureCapacityForNewElement()) [[likely]] {
            new (memory.get() + size) T{ value };
            ++size;
            return true;
        }
        return false;
    }

    [[nodiscard]] T& back() noexcept
    {
        assert(size > 0);
        return elementAt(size - 1);
    }

    void popBack() noexcept
    {
        assert(size > 0);
        std::destroy_at(&back());
        --size;
    }

    void fastRemoveAt(std::size_t index) noexcept
    {
        elementAt(index) = back();
        popBack();
    }
    
    void clear() noexcept
    {
        std::destroy_n(memory.get(), size);
        size = 0;
    }

private:
    [[nodiscard]] static std::size_t calculateNewCapacity(std::size_t currentCapacity) noexcept
    {
        return currentCapacity + currentCapacity / 2 + 1;
    }

    [[nodiscard]] T& elementAt(std::size_t index) noexcept
    {
        assert(index < size);
        return memory.get()[index];
    }

    [[nodiscard]] const T& elementAt(std::size_t index) const noexcept
    {
        assert(index < size);
        return memory.get()[index];
    }

    [[nodiscard]] std::size_t calculateNewCapacity() const noexcept
    {
        return calculateNewCapacity(getCapacity());
    }

    [[nodiscard]] bool ensureCapacityForNewElement() noexcept
    {
        return size < getCapacity() || growMemory(calculateNewCapacity());
    }

    [[nodiscard]] bool growMemory(std::size_t newCapacity) noexcept
    {
        assert(newCapacity > getCapacity());
        if (const auto newMemory = createNewMemory(newCapacity)) {
            memory.reset(newMemory);
            memory.get_deleter() = MemoryDeleter<T[]>(newCapacity);
            return true;
        }
        return false;
    }

    [[nodiscard]] T* createNewMemory(std::size_t newCapacity) const noexcept
    {
        if (const auto newMemory = MemoryAllocator<T[]>::allocate(newCapacity)) {
            copyMemoryTo(newMemory);
            return reinterpret_cast<T*>(newMemory);
        }
        return nullptr;
    }

    void copyMemoryTo(std::byte* newMemory) const noexcept
    {
        if (size > 0)
            std::memcpy(newMemory, memory.get(), size * sizeof(T));
    }

    UniquePtr<T[]> memory;
    std::size_t size{0};
};
