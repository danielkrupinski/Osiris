#pragma once

#include <cstdint>

#include <CS2/Panorama/CImagePanel.h>
#include <GameClasses/OffsetTypes/PanoramaImagePanelOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanoramaImagePanelPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CImagePanel::SetImage* setImage() const noexcept
    {
        return patternFinders.clientPatternFinder("88 85 ? ? ? ? E8 ? ? ? ? 48 83 BD ? ? ? ? ? 74 08 4C 89 EF E8 ? ? ? ? 48 83 BD ? ? ? ? ? 0F 84"_pat).add(7).abs().template as<cs2::CImagePanel::SetImage*>();
    }

    [[nodiscard]] cs2::CImagePanel::Constructor* constructor() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 4C 8B 85 ? ? ? ? 4C 89 E6 48 8B BD"_pat).add(1).abs().template as<cs2::CImagePanel::Constructor*>();
    }

    [[nodiscard]] std::uint32_t* size() const noexcept
    {
        return patternFinders.clientPatternFinder("BF ? ? ? ? 4C 89 85 ? ? ? ? E8 ? ? ? ? 48 8B B5"_pat).add(1).template as<std::uint32_t*>();
    }

    [[nodiscard]] ImagePropertiesOffset imagePropertiesOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("4C 89 E7 49 8D 4C 24 ?"_pat).add(7).template readOffset<ImagePropertiesOffset>();
    }

    [[nodiscard]] OffsetToImagePath offsetToImagePath() const noexcept
    {
        return patternFinders.clientPatternFinder("48 81 C7 ? ? ? ? 53 89 F3 48"_pat).add(3).template readOffset<OffsetToImagePath>();
    }
};
