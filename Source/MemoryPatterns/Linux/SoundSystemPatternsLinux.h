#pragma once

#include <CS2/Classes/Sound.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SoundSystemPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::SoundChannels** soundChannels() const noexcept
    {
        return patternFinders.soundSystemPatternFinder("4C 8D 25 ? ? ? ? 48 B8"_pat).add(3).abs().template as<cs2::SoundChannels**>();
    }
};
