#pragma once

#include <MemoryPatterns/PatternTypes/SoundSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SoundSystemPatterns {
    [[nodiscard]] static consteval auto addSoundSystemPatterns(auto soundSystemPatterns) noexcept
    {
        return soundSystemPatterns
            .template addPattern<SoundChannelsPointer, CodePattern{"35 ? ? ? ? 41 8B F9"}.add(1).abs()>();
    }
};
