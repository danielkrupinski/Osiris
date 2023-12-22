#pragma once

#include <cstdint>

#include <CS2/Classes/IMemAlloc.h>

struct MemAllocPatterns {
    [[nodiscard]] std::int8_t* allocOffset() const noexcept;
};
