#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanoramaUiPanelPatterns.h>

inline std::int32_t* PanoramaUiPanelPatterns::setParent() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8B B8 ? ? ? ? 48 85 D2"_pat).add(3).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::setVisible() noexcept
{
    return GlobalContext::instance().clientPatternFinder("FF 90 ? ? ? ? 45 33 C0 33 D2 ? 8B CE"_pat).add(2).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::findChildInLayoutFile() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8B 48 ? 48 8D 15 ? ? ? ? 4C 89 74 24 ? 48 8B 01 FF 90 ? ? ? ? 48"_pat).add(21).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::getAttributeString() noexcept
{
    return GlobalContext::instance().clientPatternFinder("12 48 8B 01 FF 90 ? ? ? ? 48 8B D0 48 85 C0"_pat).add(6).as<std::int32_t*>();
}

inline std::int32_t* PanoramaUiPanelPatterns::setAttributeString() noexcept
{
    return GlobalContext::instance().clientPatternFinder("FF 90 ? ? ? ? 48 83 C6 ? 48 3B"_pat).add(2).as<std::int32_t*>();
}

inline ChildPanelsVectorOffset PanoramaUiPanelPatterns::childPanelsVectorOffset() noexcept
{
    return ChildPanelsVectorOffset{
        GlobalContext::instance().panoramaPatternFinder("83 7B ? 00 0F 84 ? ? ? ? 48 8B 4B"_pat).add(2).as<std::int8_t*>()
    };
}

inline PanelClassesVectorOffset PanoramaUiPanelPatterns::classesVectorOffset() noexcept
{
    return PanelClassesVectorOffset{
        GlobalContext::instance().panoramaPatternFinder("48 63 88 ? ? ? ? 45 33 C0"_pat).add(3).as<std::int32_t*>()
    };
}

inline PanelStyleOffset PanoramaUiPanelPatterns::panelStyleOffset() noexcept
{
    return PanelStyleOffset{
        GlobalContext::instance().panoramaPatternFinder("D2 E8 ? ? ? ? 48 8D 43 ? 48"_pat).add(9).as<std::int8_t*>()
    };
}
