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

    [[nodiscard]] OffsetToGlowSceneObjectAttachedSceneObject offsetToGlowSceneObjectAttachedSceneObject() const noexcept
    {
        return patternFinders.clientPatternFinder("89 98 ? ? ? ? 49 8B ? 48"_pat).add(2).template readOffset<OffsetToGlowSceneObjectAttachedSceneObject>();
    }
};
