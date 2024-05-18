#pragma once

#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <GameClasses/OffsetTypes/EntitySystemOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct EntitySystemPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CGameEntitySystem** entitySystem() const noexcept
    {
        return patternFinders.clientPatternFinder("48 81 EC ? ? ? ? 4C 8D 2D ? ? ? ? EB"_pat).add(10).abs().template as<cs2::CGameEntitySystem**>();
    }

    [[nodiscard]] HighestEntityIndexOffset highestEntityIndexOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("39 97 ? ? ? ? 7D 06"_pat).add(2).template readOffset<HighestEntityIndexOffset>();
    }

    [[nodiscard]] EntityListOffset entityListOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("4C 8D 6F ? 41 54 53 48 89 FB 48 83 EC ? 48 89 07 48"_pat).add(3).template readOffset<EntityListOffset>();
    }
};
