#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/ClientModePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline ZoomedSniperEffectWeightOffset ClientModePatterns::zoomedSniperEffectWeightOffset() noexcept
{
    return ZoomedSniperEffectWeightOffset{
        GlobalContext::instance().clientPatternFinder("C7 87 ? ? ? ? 00 00 80 ? EB 0A"_pat).add(2).as<std::int32_t*>()
    };
}

inline ZoomedMovingSniperEffectWeightOffset ClientModePatterns::zoomedMovingSniperEffectWeightOffset() noexcept
{
    return ZoomedMovingSniperEffectWeightOffset{
        GlobalContext::instance().clientPatternFinder("C7 87 ? ? ? ? 00 00 80 ? EB 16"_pat).add(2).as<std::int32_t*>()
    };
}
