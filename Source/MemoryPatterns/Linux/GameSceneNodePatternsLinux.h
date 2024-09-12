#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameSceneNodePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToAbsOrigin offsetToAbsOrigin() const noexcept
    {
        return patternFinders.clientPatternFinder("43 ? F3 0F 10 83 ? ? ? ? 66"_pat).add(6).template readOffset<OffsetToAbsOrigin>();
    }

    [[nodiscard]] OffsetToGameSceneNodeOwner offsetToOwner() const noexcept
    {
        return patternFinders.clientPatternFinder("4C 8B 6F ? ? F6 ? ? 0F"_pat).add(3).template readOffset<OffsetToGameSceneNodeOwner>();
    }

    [[nodiscard]] OffsetToChildGameSceneNode offsetToChild() const noexcept
    {
        return patternFinders.clientPatternFinder("? 48 85 DB 75 ? EB ? 0F B6"_pat).template readOffset<OffsetToChildGameSceneNode>();
    }

    [[nodiscard]] OffsetToNextSiblingGameSceneNode offsetToNextSibling() const noexcept
    {
        return patternFinders.clientPatternFinder("50 ? 48 8B 5B ? 48 85 DB 0F"_pat).add(5).template readOffset<OffsetToNextSiblingGameSceneNode>();
    }
};
