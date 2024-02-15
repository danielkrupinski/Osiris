#pragma once

#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CImagePanel::setImage PanoramaImagePanelPatterns::setImage() const noexcept
{
    return clientPatternFinder("24 ? 48 85 F6 48 0F 44 F0 E8 ? ? ? ? 48 83 7D ? 00 0F 84"_pat).add(10).abs().as<cs2::CImagePanel::setImage>();
}

inline cs2::CImagePanel::Constructor* PanoramaImagePanelPatterns::constructor() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 4C 8B 85 ? ? ? ? 4C 89 E6 48 8B BD"_pat).add(1).abs().as<cs2::CImagePanel::Constructor*>();
}

inline std::uint32_t* PanoramaImagePanelPatterns::size() const noexcept
{
    return clientPatternFinder("BF ? ? ? ? 4C 89 85 ? ? ? ? E8 ? ? ? ? 48 8B B5"_pat).add(1).as<std::uint32_t*>();
}
