#pragma once

#include <cstdint>

#include <GameClasses/OffsetTypes/PanoramaUiPanelOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanoramaUiPanelPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] std::int32_t* setParent() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 90 ? ? ? ? 48 85 F6 74 16"_pat).add(3).template as<std::int32_t*>();
    }

    [[nodiscard]] std::int32_t* setVisible() const noexcept
    {
        return patternFinders.clientPatternFinder("F6 48 8B 78 ? 48 8B 07 FF 90 ? ? ? ? E9 ? ? ? ? ? 8D"_pat).add(10).template as<std::int32_t*>();
    }

    [[nodiscard]] std::int32_t* findChildInLayoutFile() const noexcept
    {
        return patternFinders.clientPatternFinder("49 89 84 24 ? ? ? ? 48 8B 07 FF 90 ? ? ? ? 49 8B 7C 24 ? 48 8D 35 ? ? ? ? 48 8B 07 FF 90 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 89 C7 48 8B 00 FF 50 50 49 89 C6 48 8D 05 ? ? ? ? 0F B7 18"_pat).add(34).template as<std::int32_t*>();
    }

    [[nodiscard]] std::int32_t* getAttributeString() const noexcept
    {
        return patternFinders.clientPatternFinder("FF 90 ? ? ? ? 41 80 BC 24 ? ? ? ? ? 48 89 C2"_pat).add(2).template as<std::int32_t*>();
    }

    [[nodiscard]] std::int32_t* setAttributeString() const noexcept
    {
        return patternFinders.clientPatternFinder("FF 90 ? ? ? ? 8B 05 ? ? ? ? 39 85 ? ? ? ? 0F 84 ? ? ? ? 48 8B 3B"_pat).add(2).template as<std::int32_t*>();
    }

    [[nodiscard]] ChildPanelsVectorOffset childPanelsVectorOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("0F 85 ? ? ? ? 8B ? ? 85 D2 0F 84"_pat).add(8).template readOffset<ChildPanelsVectorOffset>();
    }

    [[nodiscard]] PanelClassesVectorOffset classesVectorOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("74 33 8B 97 ? ? ? ? 85"_pat).add(4).template readOffset<PanelClassesVectorOffset>();
    }

    [[nodiscard]] PanelStyleOffset panelStyleOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("E8 ? ? ? ? 48 8D 43 ? 48 8B"_pat).add(8).template readOffset<PanelStyleOffset>();
    }

    [[nodiscard]] ParentWindowOffset parentWindowOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("4D 89 ? 24 ? 4D 85 ? 0F 84 ? ? ? ? 4C"_pat).add(4).template readOffset<ParentWindowOffset>();
    }
};

