#pragma once

#include <MemoryPatterns/HostageServicesPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToCarriedHostage HostageServicesPatterns::offsetToCarriedHostage() const noexcept
{
    return clientPatternFinder("8B 48 ? 83 F9 FF 74 ? 4D 85 C0"_pat).add(2).readOffset<OffsetToCarriedHostage>();
}
