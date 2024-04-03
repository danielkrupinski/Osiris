#pragma once

#include <array>

#include <CS2/Classes/Panorama.h>
#include <Platform/VmtFinder.h>
#include <Utils/TypeIndex.h>

#include "StylePropertySymbolMap.h"

template <typename... StyleProperties>
struct StylePropertiesSymbolsAndVMTsBase {
    StylePropertiesSymbolsAndVMTsBase(StylePropertySymbolMap symbolMap, const VmtFinder& panoramaVmtFinder) noexcept
        : symbols{symbolMap.findSymbol(StyleProperties::kName)...}
        , vmts{panoramaVmtFinder.findVmt(StyleProperties::kMangledTypeName)...}
    {
    }

    template <typename StyleProperty>
    [[nodiscard]] cs2::CStyleSymbol getSymbol() const noexcept
    {
        return symbols[utils::typeIndex<StyleProperty, std::tuple<StyleProperties...>>()];
    }

    template <typename StyleProperty>
    [[nodiscard]] const void* getVmt() const noexcept
    {
        return vmts[utils::typeIndex<StyleProperty, std::tuple<StyleProperties...>>()];
    }

private:
    std::array<cs2::CStyleSymbol, sizeof...(StyleProperties)> symbols;
    std::array<const void*, sizeof...(StyleProperties)> vmts;
};

using StylePropertiesSymbolsAndVMTs =
    StylePropertiesSymbolsAndVMTsBase<
        cs2::CStylePropertyWidth,
        cs2::CStylePropertyOpacity,
        cs2::CStylePropertyZIndex,
        cs2::CStylePropertyHeight,
        cs2::CStylePropertyImageShadow,
        cs2::CStylePropertyPosition,
        cs2::CStylePropertyTransformOrigin,
        cs2::CStylePropertyAlign,
        cs2::CStylePropertyWashColor,
        cs2::CStylePropertyFlowChildren,
        cs2::CStylePropertyFont,
        cs2::CStylePropertyTextShadow,
        cs2::CStylePropertyMargin>;
