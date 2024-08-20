#pragma once

#include <cstdint>

#include <CS2/Panorama/CLabel.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanoramaLabelPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CLabel::Constructor* constructor() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 48 8B D8 48 8D 3D ? ? ? ? EB"_pat).add(1).abs().template as<cs2::CLabel::Constructor*>();
    }

    [[nodiscard]] cs2::CLabel::setTextInternal setTextInternal() const noexcept
    {
        return patternFinders.clientPatternFinder("41 B1 01 41 B8 ? ? ? ? E9 ? ? ? ?"_pat).add(10).abs().template as<cs2::CLabel::setTextInternal>();
    }

    [[nodiscard]] std::uint32_t* size() const noexcept
    {
        return patternFinders.clientPatternFinder("B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 45 33 C0 49 8B D4 48 8B C8 E8 ? ? ? ? 48 8B D8 48 8D"_pat).add(1).template as<std::uint32_t*>();
    }
};
