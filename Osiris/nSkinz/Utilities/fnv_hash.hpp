#pragma once

#include <cstdint>

namespace fnv {
    constexpr uint32_t offsetBasis = 0x811c9dc5;
    constexpr uint32_t prime = 0x1000193;

    constexpr uint32_t hash(const char* str, const uint32_t value = offsetBasis) noexcept
    {
        return *str ? hash(str + 1, (value ^ *str) * static_cast<unsigned long long>(prime)) : value;
    }

    constexpr uint32_t hashRuntime(const char* str) noexcept
    {
        auto value = offsetBasis;

        while (*str) {
            value ^= *str++;
            value *= prime;
        }
        return value;
    }
}
