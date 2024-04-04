#pragma once

#include <cstdint>

#include <MemoryPatterns/TopLevelWindowPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline UiScaleFactorOffset TopLevelWindowPatterns::uiScaleFactorOffset() const noexcept
{
    return panoramaPatternFinder("53 F3 0F 11 47 ?"_pat).add(5).readOffset<UiScaleFactorOffset>();
}
