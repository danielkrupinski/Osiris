#pragma once

#include <CS2/Panorama/CPanel2D.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanelPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CPanel2D::Create* create() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B F1 48 8B FA B9 ? ? ? ? E8 ? ? ? ? 48 8B D8 48 85 C0 74 5C"_pat).template as<cs2::CPanel2D::Create*>();
    }
};
