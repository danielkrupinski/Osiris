#pragma once

#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CImagePanel::setImage PanoramaImagePanelPatterns::setImage() const noexcept
{
    return clientPatternFinder("C0 48 0F 45 D0 E8 ? ? ? ? ? 39 75"_pat).add(6).abs().as<cs2::CImagePanel::setImage>();
}

inline cs2::CImagePanel::Constructor* PanoramaImagePanelPatterns::constructor() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 48 8B F0 45 33 ED"_pat).add(1).abs().as<cs2::CImagePanel::Constructor*>();
}

inline std::uint32_t* PanoramaImagePanelPatterns::size() const noexcept
{
    return clientPatternFinder("FF 90 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 4C 8B E8"_pat).add(7).as<std::uint32_t*>();
}
