#pragma once

#include <GameClasses/OffsetTypes/GlowSceneObjectOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GlowSceneObjectPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToGlowSceneObjectEntity offsetToGlowSceneObjectEntity() const noexcept
    {
        return patternFinders.clientPatternFinder("4C 89 B0 ? ? ? ? 49 8B 44 24"_pat).add(3).template readOffset<OffsetToGlowSceneObjectEntity>();
    }

    [[nodiscard]] OffsetToSceneObjectFlags offsetToSceneObjectFlags() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("06 ? ? ? ? ? 0F 85 ? ? ? ? 41 F6 44 24 ?"_pat).add(16).template readOffset<OffsetToSceneObjectFlags>();
    }
};
