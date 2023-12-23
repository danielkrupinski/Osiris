#pragma once

#include <MemoryPatterns/SoundSystemPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::SoundChannels** SoundSystemPatterns::soundChannels() const noexcept
{
    return soundSystemPatternFinder("4C 8D 25 ? ? ? ? 48 B8"_pat).add(3).abs().as<cs2::SoundChannels**>();
}
