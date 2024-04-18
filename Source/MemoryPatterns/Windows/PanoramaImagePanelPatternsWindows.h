#pragma once

#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CImagePanel::SetImage* PanoramaImagePanelPatterns::setImage() const noexcept
{
    return clientPatternFinder("7C 07 48 8D 15 ? ? ? ? E8 ? ? ? ?"_pat).add(10).abs().as<cs2::CImagePanel::SetImage*>();
}

inline cs2::CImagePanel::Constructor* PanoramaImagePanelPatterns::constructor() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 48 8B F0 45 33 ED"_pat).add(1).abs().as<cs2::CImagePanel::Constructor*>();
}

inline std::uint32_t* PanoramaImagePanelPatterns::size() const noexcept
{
    return clientPatternFinder("FF 90 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 4C 8B E8"_pat).add(7).as<std::uint32_t*>();
}

inline ImagePropertiesOffset PanoramaImagePanelPatterns::imagePropertiesOffset() const noexcept
{
    return clientPatternFinder("EB 08 41 C7 47 ? 00 00 80 BF ? 8D ? ?"_pat).add(13).readOffset<ImagePropertiesOffset>();
}

inline OffsetToImagePath PanoramaImagePanelPatterns::offsetToImagePath() const noexcept
{
    return clientPatternFinder("B6 DA 48 81 C1 ? ? ? ?"_pat).add(5).readOffset<OffsetToImagePath>();
}
