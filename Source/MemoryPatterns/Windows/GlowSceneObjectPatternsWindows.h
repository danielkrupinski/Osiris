#pragma once

#include <GameClasses/OffsetTypes/GlowSceneObjectOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GlowSceneObjectPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToGlowSceneObjectEntity offsetToGlowSceneObjectEntity() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 B0 ? ? ? ? 48 8B ? ? 48 85"_pat).add(3).template readOffset<OffsetToGlowSceneObjectEntity>();
    }

    [[nodiscard]] OffsetToGlowSceneObjectAttachedSceneObject offsetToGlowSceneObjectAttachedSceneObject() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 ? 48 89 ? ? ? ? ? 48 8B ? 48 8B"_pat).add(6).template readOffset<OffsetToGlowSceneObjectAttachedSceneObject>();
    }
};
