#pragma once

#include <MemoryPatterns/PanoramaUiPanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline std::int32_t* PanoramaUiPanelPatterns::setParent() const noexcept
{
    return clientPatternFinder("48 8B 90 ? ? ? ? 48 85 F6 74 16"_pat).add(3).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::setVisible() const noexcept
{
    return clientPatternFinder("F6 48 8B 78 ? 48 8B 07 FF 90 ? ? ? ? E9 ? ? ? ? ? 8D"_pat).add(10).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::findChildInLayoutFile() const noexcept
{
    return clientPatternFinder("FF 90 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 89 C7 48 8B 00 FF 50 50 49 89 C6 48 8D 05 ? ? ? ? 0F B7 18"_pat).add(2).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::getAttributeString() const noexcept
{
    return clientPatternFinder("FF 90 ? ? ? ? 41 80 BC 24 ? ? ? ? ? 48 89 C2"_pat).add(2).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::setAttributeString() const noexcept
{
    return clientPatternFinder("FF 90 ? ? ? ? 8B 05 ? ? ? ? 39 85 ? ? ? ? 0F 84 ? ? ? ? 48 8B 3B"_pat).add(2).as<std::int32_t*>();
}

inline ChildPanelsVectorOffset PanoramaUiPanelPatterns::childPanelsVectorOffset() const noexcept
{
    return panoramaPatternFinder("0F 85 ? ? ? ? 8B 57 ? 85"_pat).add(8).readOffset<ChildPanelsVectorOffset>();
}

inline PanelClassesVectorOffset PanoramaUiPanelPatterns::classesVectorOffset() const noexcept
{
    return panoramaPatternFinder("74 33 8B 97 ? ? ? ? 85"_pat).add(4).readOffset<PanelClassesVectorOffset>();
}

inline PanelStyleOffset PanoramaUiPanelPatterns::panelStyleOffset() const noexcept
{
    return panoramaPatternFinder("E8 ? ? ? ? 48 8D 43 ? 48 8B"_pat).add(8).readOffset<PanelStyleOffset>();
}

inline ParentWindowOffset PanoramaUiPanelPatterns::parentWindowOffset() const noexcept
{
    return panoramaPatternFinder("4D 89 5C 24 ?"_pat).add(4).readOffset<ParentWindowOffset>();
}
