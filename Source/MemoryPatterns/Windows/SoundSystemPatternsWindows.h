#pragma once

#include <MemoryPatterns/PatternTypes/SoundSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SoundSystemPatterns {
    [[nodiscard]] static consteval auto addSoundSystemPatterns(auto soundSystemPatterns) noexcept
    {
        return soundSystemPatterns
            .template addPattern<SoundChannelsPointer, CodePattern{"8B 3D ? ? ? ? 41 8B F1 48"}.add(2).abs()>();
    }
};
