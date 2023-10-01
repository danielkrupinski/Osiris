#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/ClientModePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline ZoomedSniperEffectWeightOffset ClientModePatterns::zoomedSniperEffectWeightOffset() noexcept
{
    return ZoomedSniperEffectWeightOffset{
        GlobalContext::instance().clientPatternFinder("41 C7 84 24 ? ? ? ? 00 00 80 ? E9 ? ? ? ? 0F 2F"_pat).add(4).as<std::int32_t*>()
    };
}

inline ZoomedMovingSniperEffectWeightOffset ClientModePatterns::zoomedMovingSniperEffectWeightOffset() noexcept
{
    return ZoomedMovingSniperEffectWeightOffset{
        GlobalContext::instance().clientPatternFinder("76 ? 41 C7 84 24 ? ? ? ? 00 00 80"_pat).add(6).as<std::int32_t*>()
    };
}
