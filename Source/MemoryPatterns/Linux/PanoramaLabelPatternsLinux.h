#pragma once

#include <cstdint>

#include <CS2/Panorama/CLabel.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanoramaLabelPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CLabel::Constructor* constructor() const noexcept
    {
        return patternFinders.clientPatternFinder("49 89 C7 E8 ? ? ? ? 49 8B 7C 24 ? 48 8B 07"_pat).add(4).abs().template as<cs2::CLabel::Constructor*>();
    }

    [[nodiscard]] cs2::CLabel::setTextInternal setTextInternal() const noexcept
    {
        return patternFinders.clientPatternFinder("85 ? ? ? ? 31 C9 E8 ? ? ? ?"_pat).add(8).abs().template as<cs2::CLabel::setTextInternal>();
    }

    [[nodiscard]] std::uint32_t* size() const noexcept
    {
        return patternFinders.clientPatternFinder("BF ? ? ? ? E8 ? ? ? ? 31 D2 4C 89 E6 48 89 C7 49 89 C7 E8 ? ? ? ? 49 8B 7C"_pat).add(1).template as<std::uint32_t*>();
    }
};
