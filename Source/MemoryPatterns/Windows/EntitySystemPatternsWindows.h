#pragma once

#include <MemoryPatterns/EntitySystemPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CGameEntitySystem** EntitySystemPatterns::entitySystem() const noexcept
{
    return clientPatternFinder("48 8B 0D ? ? ? ? 48 8D 94 24 ? ? ? ? 33 DB"_pat).add(3).abs().as<cs2::CGameEntitySystem**>();
}

inline HighestEntityIndexOffset EntitySystemPatterns::highestEntityIndexOffset() const noexcept
{
    return clientPatternFinder("3B 8F ? ? ? ? 7E 06"_pat).add(2).readOffset<HighestEntityIndexOffset>();
}

inline EntityListOffset EntitySystemPatterns::entityListOffset() const noexcept
{
    return clientPatternFinder("48 8D 4B ? E8 ? ? ? ? 8D 85"_pat).add(3).readOffset<EntityListOffset>();
}
