#pragma once

#include <MemoryPatterns/PatternTypes/SoundSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SoundSystemPatterns {
    [[nodiscard]] static consteval auto addSoundSystemPatterns(auto soundSystemPatterns) noexcept
    {
        return soundSystemPatterns
            .template addPattern<SoundChannelsPointer, CodePattern{"4C 8D 25 ? ? ? ? 48 B8"}.add(3).abs()>();
    }
};
