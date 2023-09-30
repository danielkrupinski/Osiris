#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanoramaLabelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CLabel::setTextInternal PanoramaLabelPatterns::setTextInternal() noexcept
{
    return GlobalContext::instance().clientPatternFinder("41 B1 01 41 B8 ? ? ? ? E9"_pat).add(10).abs().as<cs2::CLabel::setTextInternal>();
}
