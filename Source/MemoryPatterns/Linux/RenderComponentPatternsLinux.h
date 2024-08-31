#pragma once

#include <GameClasses/OffsetTypes/RenderComponentOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct RenderComponentPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToSceneObjectUpdaters offsetToSceneObjectUpdaters() const noexcept
    {
        return patternFinders.clientPatternFinder("? 7E ? 48 8B 47 ? 48 63 F6 48 8B 34"_pat).template readOffset<OffsetToSceneObjectUpdaters>();
    }
};
