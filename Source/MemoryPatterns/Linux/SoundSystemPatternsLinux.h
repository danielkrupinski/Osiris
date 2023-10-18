#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/SoundSystemPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::SoundChannels** SoundSystemPatterns::soundChannels() noexcept
{
    return GlobalContext::instance().soundSystemPatternFinder("4C 8D 25 ? ? ? ? 48 B8"_pat).add(3).abs().as<cs2::SoundChannels**>();
}
