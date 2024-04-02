#pragma once

#include <MemoryPatterns/PanoramaLabelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CLabel::Constructor* PanoramaLabelPatterns::constructor() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 48 8B D8 48 8D 3D ? ? ? ? EB"_pat).add(1).abs().as<cs2::CLabel::Constructor*>();
}

inline cs2::CLabel::setTextInternal PanoramaLabelPatterns::setTextInternal() const noexcept
{
    return clientPatternFinder("41 B1 01 41 B8 ? ? ? ? E9 ? ? ? ?"_pat).add(10).abs().as<cs2::CLabel::setTextInternal>();
}

inline std::uint32_t* PanoramaLabelPatterns::size() const noexcept
{
    return clientPatternFinder("B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 45 33 C0 49 8B D4 48 8B C8 E8 ? ? ? ? 48 8B D8 48 8D"_pat).add(1).as<std::uint32_t*>();
}
