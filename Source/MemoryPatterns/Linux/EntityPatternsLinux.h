#pragma once

#include <GameClasses/OffsetTypes/EntityOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct EntityPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToGameSceneNode offsetToGameSceneNode() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 84 C0 75 ? 49 8B BC 24 ? ? ? ? 4C 89 EE"_pat).add(13).template readOffset<OffsetToGameSceneNode>();
    }

    [[nodiscard]] OffsetToHealth offsetToHealth() const noexcept
    {
        return patternFinders.clientPatternFinder("8B 87 ? ? ? ? 85 C0 7E ? C7"_pat).add(2).template readOffset<OffsetToHealth>();
    }

    [[nodiscard]] OffsetToLifeState offsetToLifeState() const noexcept
    {
        return patternFinders.clientPatternFinder("0F B6 97 ? ? ? ? 39 F2"_pat).add(3).template readOffset<OffsetToLifeState>();
    }

    [[nodiscard]] OffsetToTeamNumber offsetToTeamNumber() const noexcept
    {
        return patternFinders.clientPatternFinder("41 0F B6 84 24 ? ? ? ? 3C 03"_pat).add(5).template readOffset<OffsetToTeamNumber>();
    }

    [[nodiscard]] OffsetToVData offsetToVData() const noexcept
    {
        return patternFinders.clientPatternFinder("49 8B 84 24 ? ? ? ? 5A"_pat).add(4).template readOffset<OffsetToVData>();
    }
};
