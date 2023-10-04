#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanoramaLabelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CLabel::setTextInternal PanoramaLabelPatterns::setTextInternal() noexcept
{
    return GlobalContext::instance().clientPatternFinder("31 C9 E8 ? ? ? ? E9"_pat).add(3).abs().as<cs2::CLabel::setTextInternal>();
}
