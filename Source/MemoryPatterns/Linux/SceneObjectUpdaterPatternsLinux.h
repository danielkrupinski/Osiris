#pragma once

#include <GameClasses/OffsetTypes/SceneObjectUpdaterOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SceneObjectUpdaterPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToSceneObject offsetToSceneObject() const noexcept
    {
        return patternFinders.clientPatternFinder("74 ? 48 8B 46 ? C3"_pat).add(5).template readOffset<OffsetToSceneObject>();
    }
};
