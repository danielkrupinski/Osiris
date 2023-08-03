#include "MockMemoryAllocator.h"
#include "MemoryAllocation/MemoryAllocatorBase.h"

namespace
{
    std::weak_ptr<MockMemoryAllocator::MockType> mockMemoryAllocator;
}

[[nodiscard]] std::shared_ptr<MockMemoryAllocator::MockType> MockMemoryAllocator::create()
{
    if (const auto existingMock = mockMemoryAllocator.lock())
        return existingMock;

    auto mock = std::make_shared<MockMemoryAllocator::MockType>();
    mockMemoryAllocator = mock;
    return mock;
}

std::byte* MemoryAllocatorBase::allocate(std::size_t size) noexcept
{
    if (const auto mock = mockMemoryAllocator.lock())
        return mock->allocate(size);
    return nullptr;
}

void MemoryAllocatorBase::deallocate(std::byte* memory, std::size_t size) noexcept
{
    if (const auto mock = mockMemoryAllocator.lock())
        mock->deallocate(memory, size);
}
