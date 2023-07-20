#pragma once

#include <cstddef>
#include <memory>

#include <gmock/gmock.h>

struct MockMemoryAllocator {
    MOCK_METHOD(std::byte*, allocate, (std::size_t size), (noexcept));
    MOCK_METHOD(void, deallocate, (std::byte* memory, std::size_t size), (noexcept));

    using MockType = testing::StrictMock<MockMemoryAllocator>;
    [[nodiscard]] static std::shared_ptr<MockType> create();
};
