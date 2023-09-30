#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CImagePanel::setImage PanoramaImagePanelPatterns::setImage() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 85 F6 48 0F 44 F0 E8 ? ? ? ? 48 83 7D ? 00 0F 84 83"_pat).add(8).abs().as<cs2::CImagePanel::setImage>();
}
