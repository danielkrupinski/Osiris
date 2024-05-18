#pragma once

#include <cstdint>

#include <CS2/Classes/Panorama.h>
#include <GameClasses/OffsetTypes/PanoramaImagePanelOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanoramaImagePanelPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CImagePanel::SetImage* setImage() const noexcept
    {
        return patternFinders.clientPatternFinder("C7 44 24 ? ? ? ? ? E8 ? ? ? ? 48 83 7C 24 ? ? 0F 84"_pat).add(9).abs().template as<cs2::CImagePanel::SetImage*>();
    }

    [[nodiscard]] cs2::CImagePanel::Constructor* constructor() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 48 8B F0 45 33 ED"_pat).add(1).abs().template as<cs2::CImagePanel::Constructor*>();
    }

    [[nodiscard]] std::uint32_t* size() const noexcept
    {
        return patternFinders.clientPatternFinder("FF 90 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 4C 8B E8"_pat).add(7).template as<std::uint32_t*>();
    }

    [[nodiscard]] ImagePropertiesOffset imagePropertiesOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("EB 08 41 C7 47 ? 00 00 80 BF ? 8D ? ?"_pat).add(13).template readOffset<ImagePropertiesOffset>();
    }

    [[nodiscard]] OffsetToImagePath offsetToImagePath() const noexcept
    {
        return patternFinders.clientPatternFinder("B6 DA 48 81 C1 ? ? ? ?"_pat).add(5).template readOffset<OffsetToImagePath>();
    }
};
