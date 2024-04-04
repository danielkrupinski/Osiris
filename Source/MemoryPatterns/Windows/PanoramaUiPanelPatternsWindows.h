#pragma once

#include <MemoryPatterns/PanoramaUiPanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline std::int32_t* PanoramaUiPanelPatterns::setParent() const noexcept
{
    return clientPatternFinder("48 8B B8 ? ? ? ? 48 85 D2"_pat).add(3).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::setVisible() const noexcept
{
    return clientPatternFinder("FF 90 ? ? ? ? 45 33 C0 33 D2 ? 8B ? E8"_pat).add(2).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::findChildInLayoutFile() const noexcept
{
    return clientPatternFinder("48 8B 48 ? 48 8D 15 ? ? ? ? 4C 89 74 24 ? 48 8B 01 FF 90 ? ? ? ? 48"_pat).add(21).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::getAttributeString() const noexcept
{
    return clientPatternFinder("12 48 8B 01 FF 90 ? ? ? ? 48 8B D0 48 85 C0"_pat).add(6).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::setAttributeString() const noexcept
{
    return clientPatternFinder("FF 90 ? ? ? ? 48 83 C6 ? 48 3B"_pat).add(2).as<std::int32_t*>();
}

inline ChildPanelsVectorOffset PanoramaUiPanelPatterns::childPanelsVectorOffset() const noexcept
{
    return panoramaPatternFinder("83 7B ? 00 0F 84 ? ? ? ? 48 8B 4B"_pat).add(2).readOffset<ChildPanelsVectorOffset>();
}

inline PanelClassesVectorOffset PanoramaUiPanelPatterns::classesVectorOffset() const noexcept
{
    return panoramaPatternFinder("48 63 88 ? ? ? ? 45 33 C0"_pat).add(3).readOffset<PanelClassesVectorOffset>();
}

inline PanelStyleOffset PanoramaUiPanelPatterns::panelStyleOffset() const noexcept
{
    return panoramaPatternFinder("D2 E8 ? ? ? ? 48 8D 43 ? 48"_pat).add(9).readOffset<PanelStyleOffset>();
}

inline ParentWindowOffset PanoramaUiPanelPatterns::parentWindowOffset() const noexcept
{
    return panoramaPatternFinder("48 89 77 ? 48 85 F6 75"_pat).add(3).readOffset<ParentWindowOffset>();
}
