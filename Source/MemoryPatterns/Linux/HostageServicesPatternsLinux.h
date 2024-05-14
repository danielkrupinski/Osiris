#pragma once

#include <MemoryPatterns/HostageServicesPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToCarriedHostage HostageServicesPatterns::offsetToCarriedHostage() const noexcept
{
    return clientPatternFinder("44 8B 4E ? 89 C7"_pat).add(3).readOffset<OffsetToCarriedHostage>();
}
