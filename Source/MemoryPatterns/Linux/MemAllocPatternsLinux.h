#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/MemAllocPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline std::int8_t* MemAllocPatterns::allocOffset() noexcept
{
    return GlobalContext::instance().clientPatternFinder("E6 06 FF 50 ? 48 89 43"_pat).add(4).as<std::int8_t*>();
}
