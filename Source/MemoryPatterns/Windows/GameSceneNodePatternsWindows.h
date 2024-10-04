#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameSceneNodePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToAbsOrigin offsetToAbsOrigin() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 11 97 ? ? ? ? 0F 28 C2"_pat).add(4).template readOffset<OffsetToAbsOrigin>();
    }

    [[nodiscard]] OffsetToGameSceneNodeOwner offsetToOwner() const noexcept
    {
        return patternFinders.clientPatternFinder("44 39 B9 ? ? ? ? 75 ? 48 8B ? ?"_pat).add(12).template readOffset<OffsetToGameSceneNodeOwner>();
    }

    [[nodiscard]] OffsetToChildGameSceneNode offsetToChild() const noexcept
    {
        return patternFinders.clientPatternFinder("74 ? 49 8B 5E ? 48 85"_pat).add(5).template readOffset<OffsetToChildGameSceneNode>();
    }

    [[nodiscard]] OffsetToNextSiblingGameSceneNode offsetToNextSibling() const noexcept
    {
        return patternFinders.clientPatternFinder("? 48 85 DB 75 ? 83 3D"_pat).template readOffset<OffsetToNextSiblingGameSceneNode>();
    }
};
