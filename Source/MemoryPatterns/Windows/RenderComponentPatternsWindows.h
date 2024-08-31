#pragma once

#include <GameClasses/OffsetTypes/RenderComponentOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct RenderComponentPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToSceneObjectUpdaters offsetToSceneObjectUpdaters() const noexcept
    {
        return patternFinders.clientPatternFinder("? 7C 03 33 C0 C3 48 8B 41"_pat).template readOffset<OffsetToSceneObjectUpdaters>();
    }
};
