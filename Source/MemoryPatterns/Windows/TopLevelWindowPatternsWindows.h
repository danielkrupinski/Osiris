#pragma once

#include <cstdint>

#include <MemoryPatterns/TopLevelWindowPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline UiScaleFactorOffset TopLevelWindowPatterns::uiScaleFactorOffset() const noexcept
{
    return panoramaPatternFinder("F3 0F 11 49 ? 48 8B F9"_pat).add(4).readOffset<UiScaleFactorOffset>();
}
