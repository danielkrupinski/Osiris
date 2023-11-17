#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CImagePanel::setImage PanoramaImagePanelPatterns::setImage() noexcept
{
    return GlobalContext::instance().clientPatternFinder("24 ? 48 85 F6 48 0F 44 F0 E8 ? ? ? ? 48 83 7D ? 00 0F 84"_pat).add(10).abs().as<cs2::CImagePanel::setImage>();
}
