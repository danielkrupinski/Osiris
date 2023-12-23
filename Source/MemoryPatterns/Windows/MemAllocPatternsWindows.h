#pragma once

#include <MemoryPatterns/MemAllocPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline std::int8_t* MemAllocPatterns::allocOffset() const noexcept
{
    return clientPatternFinder("FF 50 ? 48 89 43 ? 4C 8D"_pat).add(2).as<std::int8_t*>();
}
