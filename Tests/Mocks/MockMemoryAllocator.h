#pragma once

#include <cstddef>
#include <memory>

#include <gmock/gmock.h>

#include <MemoryAllocation/MemoryAllocatorBase.h>

struct MockMemoryAllocator {
    MOCK_METHOD(std::byte*, allocate, (std::size_t size), (noexcept));
    MOCK_METHOD(void, deallocate, (std::byte* memory, std::size_t size), (noexcept));

    using MockType = testing::StrictMock<MockMemoryAllocator>;
    [[nodiscard]] static std::shared_ptr<MockType> create()
    {
        if (const auto existingMock = mockMemoryAllocator.lock())
            return existingMock;

        auto mock = std::make_shared<MockMemoryAllocator::MockType>();
        mockMemoryAllocator = mock;
        return mock;
    }

    static std::byte* invokeAllocate(std::size_t size)
    {
        if (const auto mock = mockMemoryAllocator.lock())
            return mock->allocate(size);
        return nullptr;
    }

    static void invokeDeallocate(std::byte* memory, std::size_t size)
    {
        if (const auto mock = mockMemoryAllocator.lock())
            mock->deallocate(memory, size);
    }

private:
    inline static std::weak_ptr<MockMemoryAllocator::MockType> mockMemoryAllocator;
};

std::byte* MemoryAllocatorBase::allocate(std::size_t size) noexcept
{
    return MockMemoryAllocator::invokeAllocate(size);
}

void MemoryAllocatorBase::deallocate(std::byte* memory, std::size_t size) noexcept
{
    MockMemoryAllocator::invokeDeallocate(memory, size);
}
