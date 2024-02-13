#pragma once

#include <MemoryPatterns/PanoramaLabelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CLabel::setTextInternal PanoramaLabelPatterns::setTextInternal() const noexcept
{
    return clientPatternFinder("41 B1 01 41 B8 ? ? ? ? E9 ? ? ? ?"_pat).add(10).abs().as<cs2::CLabel::setTextInternal>();
}
