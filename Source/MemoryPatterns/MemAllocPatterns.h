#pragma once

#include <cstdint>

#include <CS2/Classes/IMemAlloc.h>

struct MemAllocPatterns {
    [[nodiscard]] static std::int8_t* allocOffset() noexcept;
};
