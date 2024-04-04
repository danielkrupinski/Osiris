#pragma once

#include <MemoryPatterns/EntitySystemPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CGameEntitySystem** EntitySystemPatterns::entitySystem() const noexcept
{
    return clientPatternFinder("48 81 EC ? ? ? ? 4C 8D 2D ? ? ? ? EB"_pat).add(10).abs().as<cs2::CGameEntitySystem**>();
}

inline HighestEntityIndexOffset EntitySystemPatterns::highestEntityIndexOffset() const noexcept
{
    return clientPatternFinder("39 97 ? ? ? ? 7D 06"_pat).add(2).readOffset<HighestEntityIndexOffset>();
}

inline EntityListOffset EntitySystemPatterns::entityListOffset() const noexcept
{
    return clientPatternFinder("4C 8D 6F ? 41 54 53 48 89 FB 48 83 EC ? 48 89 07 48"_pat).add(3).readOffset<EntityListOffset>();
}
