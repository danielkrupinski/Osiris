#pragma once

#include <MemoryPatterns/PatternTypes/SoundSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SoundSystemPatterns {
    [[nodiscard]] static consteval auto addSoundSystemPatterns(auto soundSystemPatterns) noexcept
    {
        return soundSystemPatterns
            .template addPattern<SoundChannelsPointer, CodePattern{"B8 ? ? ? ? 00 0F 84 ? ? ? ? ? ? ? ? ? ? ? 48 B8"}.add(15).abs()>();
    }
};
