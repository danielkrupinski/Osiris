#pragma once

#include <GameClasses/OffsetTypes/SceneObjectOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SceneObjectPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToSceneObjectFlags offsetToSceneObjectFlags() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("06 ? ? ? ? ? 0F 85 ? ? ? ? 41 F6 44 24 ?"_pat).add(16).template readOffset<OffsetToSceneObjectFlags>();
    }

    [[nodiscard]] OffsetToSceneObjectClass offsetToSceneObjectClass() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("B6 46 ? 49 89 F4"_pat).add(2).template readOffset<OffsetToSceneObjectClass>();
    }
};
