#pragma once

#include <GameClasses/OffsetTypes/EntityOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct EntityPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToGameSceneNode offsetToGameSceneNode() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 8B ? ? ? ? 48 8B D7 48 8B 5C"_pat).add(3).template readOffset<OffsetToGameSceneNode>();
    }

    [[nodiscard]] OffsetToHealth offsetToHealth() const noexcept
    {
        return patternFinders.clientPatternFinder("D9 ? ? C7 81 ? ? ? ? 00 00 00 00 48 8D 15"_pat).add(5).template readOffset<OffsetToHealth>();
    }

    [[nodiscard]] OffsetToLifeState offsetToLifeState() const noexcept
    {
        return patternFinders.clientPatternFinder("0F B6 81 ? ? ? ? 3B C2"_pat).add(3).template readOffset<OffsetToLifeState>();
    }

    [[nodiscard]] OffsetToTeamNumber offsetToTeamNumber() const noexcept
    {
        return patternFinders.clientPatternFinder("80 BB ? ? ? ? 03 75"_pat).add(2).template readOffset<OffsetToTeamNumber>();
    }

    [[nodiscard]] OffsetToVData offsetToVData() const noexcept
    {
        return patternFinders.clientPatternFinder("49 8B 86 ? ? ? ? 48 85 C0 74 ? 48 8B 40 ? 48 85 C0 74 ? 80"_pat).add(3).template readOffset<OffsetToVData>();
    }

    [[nodiscard]] OffsetToRenderComponent offsetToRenderComponent() const noexcept
    {
        return patternFinders.clientPatternFinder("8B ? ? ? ? ? 48 85 C0 74 ? 39 ? ? 7E"_pat).add(2).template readOffset<OffsetToRenderComponent>();
    }

    [[nodiscard]] OffsetToOwnerEntity offsetToOwnerEntity() const noexcept
    {
        return patternFinders.clientPatternFinder("75 ? 8B 8E ? ? ? ? 3B CB"_pat).add(4).template readOffset<OffsetToOwnerEntity>();
    }
};
