#pragma once

#include <MemoryPatterns/ClientModePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline ZoomedSniperEffectWeightOffset ClientModePatterns::zoomedSniperEffectWeightOffset() const noexcept
{
    return ZoomedSniperEffectWeightOffset{
        clientPatternFinder("C7 87 ? ? ? ? 00 00 80 ? EB 0A"_pat).add(2).as<std::int32_t*>()
    };
}

inline ZoomedMovingSniperEffectWeightOffset ClientModePatterns::zoomedMovingSniperEffectWeightOffset() const noexcept
{
    return ZoomedMovingSniperEffectWeightOffset{
        clientPatternFinder("C7 87 ? ? ? ? 00 00 80 ? EB 16"_pat).add(2).as<std::int32_t*>()
    };
}
