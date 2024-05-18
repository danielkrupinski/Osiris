#pragma once

#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <GameClasses/OffsetTypes/EntitySystemOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct EntitySystemPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CGameEntitySystem** entitySystem() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 0D ? ? ? ? 48 8D 94 24 ? ? ? ? 33 DB"_pat).add(3).abs().template as<cs2::CGameEntitySystem**>();
    }

    [[nodiscard]] HighestEntityIndexOffset highestEntityIndexOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("3B 8F ? ? ? ? 7E 06"_pat).add(2).template readOffset<HighestEntityIndexOffset>();
    }

    [[nodiscard]] EntityListOffset entityListOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 4B ? E8 ? ? ? ? 8D 85"_pat).add(3).template readOffset<EntityListOffset>();
    }
};
