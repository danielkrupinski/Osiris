#pragma once

#include <MemoryPatterns/SoundSystemPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::SoundChannels** SoundSystemPatterns::soundChannels() const noexcept
{
    return soundSystemPatternFinder("4C 8B 25 ? ? ? ? 4C 89 AC 24"_pat).add(3).abs().as<cs2::SoundChannels**>();
}
