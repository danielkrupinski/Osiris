#pragma once

#include <MemoryPatterns/PanoramaLabelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CLabel::Constructor* PanoramaLabelPatterns::constructor() const noexcept
{
    return clientPatternFinder("49 89 C7 E8 ? ? ? ? 49 8B 7C 24 ? 48 8B 07"_pat).add(4).abs().as<cs2::CLabel::Constructor*>();
}

inline cs2::CLabel::setTextInternal PanoramaLabelPatterns::setTextInternal() const noexcept
{
    return clientPatternFinder("31 C9 E8 ? ? ? ? E9"_pat).add(3).abs().as<cs2::CLabel::setTextInternal>();
}

inline std::uint32_t* PanoramaLabelPatterns::size() const noexcept
{
    return clientPatternFinder("BF ? ? ? ? E8 ? ? ? ? 31 D2 4C 89 E6 48 89 C7 49 89 C7 E8 ? ? ? ? 49 8B 7C"_pat).add(1).as<std::uint32_t*>();
}
