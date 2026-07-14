#pragma once

#include <cstdint>
#include <Platform/Macros/IsPlatform.h>
#include <CS2/Panorama/CStyleSymbol.h>

#if IS_LINUX()
#include <CS2/Classes/CUtlMap.h>
#include <CS2/Classes/CUtlString.h>
#endif

namespace cs2
{

struct CStyleProperty;

struct CPanelStyle {
    using SetProperty = void(CPanelStyle* thisptr, CStyleProperty* styleProperty, bool transition);
    using FindStylePropertySymbol = std::uint8_t* (*)(std::uint8_t* outSymbol, const char* propertyName) noexcept;

#if IS_LINUX()
    using StylePropertySymbols = CUtlMap<CUtlString, CStyleSymbol>;
#endif
};

}
