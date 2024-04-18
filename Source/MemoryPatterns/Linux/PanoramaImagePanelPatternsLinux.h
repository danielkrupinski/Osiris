#pragma once

#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CImagePanel::SetImage* PanoramaImagePanelPatterns::setImage() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 48 83 7D ? ? 74 88 4C 89 FF"_pat).add(1).abs().as<cs2::CImagePanel::SetImage*>();
}

inline cs2::CImagePanel::Constructor* PanoramaImagePanelPatterns::constructor() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 4C 8B 85 ? ? ? ? 4C 89 E6 48 8B BD"_pat).add(1).abs().as<cs2::CImagePanel::Constructor*>();
}

inline std::uint32_t* PanoramaImagePanelPatterns::size() const noexcept
{
    return clientPatternFinder("BF ? ? ? ? 4C 89 85 ? ? ? ? E8 ? ? ? ? 48 8B B5"_pat).add(1).as<std::uint32_t*>();
}

inline ImagePropertiesOffset PanoramaImagePanelPatterns::imagePropertiesOffset() const noexcept
{
    return clientPatternFinder("4C 89 E7 49 8D 4C 24 ?"_pat).add(7).readOffset<ImagePropertiesOffset>();
}

inline OffsetToImagePath PanoramaImagePanelPatterns::offsetToImagePath() const noexcept
{
    return clientPatternFinder("48 81 C7 ? ? ? ? 53 89 F3 48"_pat).add(3).readOffset<OffsetToImagePath>();
}
