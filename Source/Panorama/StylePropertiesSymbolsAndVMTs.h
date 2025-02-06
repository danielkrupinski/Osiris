#pragma once

#include <array>
#include <tuple>

#include <CS2/Constants/StylePropertySymbolNames.h>
#include <CS2/Constants/StylePropertyTypeNames.h>
#include <CS2/Panorama/CStyleSymbol.h>
#include <Platform/VmtFinder.h>
#include <Utils/TypeIndex.h>

#include "StylePropertySymbolMap.h"

struct StylePropertiesSymbolsAndVMTs {
    StylePropertiesSymbolsAndVMTs() = default;

    StylePropertiesSymbolsAndVMTs(StylePropertySymbolMap symbolMap, const VmtFinder& panoramaVmtFinder) noexcept
    {
        cs2::kStylePropertySymbolNames.forEach([i = 0u, &symbolMap, this](const auto typeName) mutable { symbols[i++] = symbolMap.findSymbol(typeName); });
        cs2::kStylePropertyTypeNames.forEach([i = 0u, &panoramaVmtFinder, this](const auto typeName) mutable { vmts[i++] = panoramaVmtFinder.findVmt(typeName); });
    }

    template <typename StyleProperty>
    [[nodiscard]] cs2::CStyleSymbol getSymbol() const noexcept
    {
        return symbols[utils::typeIndex<StyleProperty, StyleProperties>()];
    }

    template <typename StyleProperty>
    [[nodiscard]] const void* getVmt() const noexcept
    {
        return vmts[utils::typeIndex<StyleProperty, StyleProperties>()];
    }

private:
    using StyleProperties = decltype(cs2::kStylePropertySymbolNames)::TypeList;

    std::array<cs2::CStyleSymbol, std::tuple_size_v<StyleProperties>> symbols;
    std::array<const void*, std::tuple_size_v<StyleProperties>> vmts;
};
