#pragma once

#include <MemoryPatterns/MemAllocPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline std::int8_t* MemAllocPatterns::allocOffset() const noexcept
{
    return clientPatternFinder("E6 06 FF 50 ? 48 89 43"_pat).add(4).as<std::int8_t*>();
}
