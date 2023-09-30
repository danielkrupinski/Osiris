#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CImagePanel::setImage PanoramaImagePanelPatterns::setImage() noexcept
{
    return GlobalContext::instance().clientPatternFinder("C0 48 0F 45 D0 E8 ? ? ? ? ? 39 75"_pat).add(6).abs().as<cs2::CImagePanel::setImage>();
}
