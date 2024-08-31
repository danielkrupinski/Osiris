#pragma once

#include <GameClasses/OffsetTypes/SceneObjectUpdaterOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SceneObjectUpdaterPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToSceneObject offsetToSceneObject() const noexcept
    {
        return patternFinders.clientPatternFinder("C3 48 8B 42 ? C3"_pat).add(4).template readOffset<OffsetToSceneObject>();
    }
};
