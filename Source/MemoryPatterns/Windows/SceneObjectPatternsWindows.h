#pragma once

#include <GameClasses/OffsetTypes/SceneObjectOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SceneObjectPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToSceneObjectFlags offsetToSceneObjectFlags() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("CC F6 83 ? ? ? ?"_pat).add(3).template readOffset<OffsetToSceneObjectFlags>();
    }

    [[nodiscard]] OffsetToSceneObjectClass offsetToSceneObjectClass() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("B6 82 ? ? ? ? 33"_pat).add(2).template readOffset<OffsetToSceneObjectClass>();
    }
};
