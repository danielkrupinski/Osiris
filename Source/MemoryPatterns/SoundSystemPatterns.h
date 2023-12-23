#pragma once

#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

namespace cs2
{
    struct SoundChannels;
}

struct SoundSystemPatterns {
    [[nodiscard]] cs2::SoundChannels** soundChannels() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& soundSystemPatternFinder;
};
