#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/MemAlloc.h>
#include "PanelShadowParams.h"
#include "StylePropertiesSymbols.h"
#include "StylePropertiesVMTs.h"

struct PanelStylePropertyFactory {
    PanelStylePropertyFactory(const StylePropertiesVMTs& stylePropertyVMTs, const StylePropertiesSymbols& stylePropertySymbols) noexcept
        : VMTs{stylePropertyVMTs}
        , symbols{stylePropertySymbols}
    {
    }

    [[nodiscard]] cs2::CStylePropertyWidth* width(cs2::CUILength width) const noexcept
    {
        return create<cs2::CStylePropertyWidth>(VMTs.widthPropertyVmt, symbols.widthSymbol, width);
    }

    [[nodiscard]] cs2::CStylePropertyOpacity* opacity(float opacity) const noexcept
    {
        return create<cs2::CStylePropertyOpacity>(VMTs.opacityPropertyVmt, symbols.opacitySymbol, opacity);
    }

    [[nodiscard]] cs2::CStylePropertyZIndex* zIndex(float zIndex) const noexcept
    {
        return create<cs2::CStylePropertyZIndex>(VMTs.zIndexPropertyVmt, symbols.zIndexSymbol, zIndex);
    }

    [[nodiscard]] cs2::CStylePropertyHeight* height(cs2::CUILength height) const noexcept
    {
        return create<cs2::CStylePropertyHeight>(VMTs.heightPropertyVmt, symbols.heightSymbol, height);
    }

    [[nodiscard]] cs2::CStylePropertyImageShadow* imageShadow(const PanelShadowParams& params) const noexcept
    {
        return create<cs2::CStylePropertyImageShadow>(VMTs.imageShadowPropertyVmt, symbols.imageShadowSymbol, true, params.horizontalOffset, params.verticalOffset, params.blurRadius, params.strength, params.color);
    }

    [[nodiscard]] cs2::CStylePropertyPosition* position(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        return create<cs2::CStylePropertyPosition>(VMTs.positionPropertyVmt, symbols.positionSymbol, x, y, cs2::CUILength::pixels(0));
    }

    [[nodiscard]] cs2::CStylePropertyTransformOrigin* transformOrigin(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        return create<cs2::CStylePropertyTransformOrigin>(VMTs.transformOriginPropertyVmt, symbols.transformOriginSymbol, x, y, false);
    }

    [[nodiscard]] cs2::CStylePropertyAlign* align(cs2::EHorizontalAlignment horizontalAlignment, cs2::EVerticalAlignment verticalAlignment) const noexcept
    {
        return create<cs2::CStylePropertyAlign>(VMTs.alignPropertyVmt, symbols.alignSymbol, horizontalAlignment, verticalAlignment);
    }

    [[nodiscard]] cs2::CStylePropertyWashColor* washColor(cs2::Color color) const noexcept
    {
        return create<cs2::CStylePropertyWashColor>(VMTs.washColorVmt, symbols.washColorSymbol, color, true);
    }

    [[nodiscard]] cs2::CStylePropertyFlowChildren* flowChildren(cs2::EFlowDirection flowDirection) const noexcept
    {
        return create<cs2::CStylePropertyFlowChildren>(VMTs.flowChildrenVmt, symbols.flowChildrenSymbol, flowDirection);
    }

    [[nodiscard]] cs2::CStylePropertyFont* font(std::string_view fontFamily, float fontSize, cs2::EFontWeight fontWeight) const noexcept
    {
        cs2::CUtlString fontFamilyString{nullptr};
        if (fontFamily.length() > 0) {
            if ((fontFamilyString.m_pString = static_cast<char*>(MemAlloc::allocate(fontFamily.length() + 1))) != nullptr) {
                std::memcpy(fontFamilyString.m_pString, fontFamily.data(), fontFamily.length());
                fontFamilyString.m_pString[fontFamily.length()] = '\0';
            }
        }
        return create<cs2::CStylePropertyFont>(VMTs.fontVmt, symbols.fontSymbol, fontFamilyString, fontSize, cs2::k_EFontStyleUnset, fontWeight, cs2::k_EFontStretchUnset);
    }

    [[nodiscard]] cs2::CStylePropertyTextShadow* textShadow(const PanelShadowParams& params) const noexcept
    {
        return create<cs2::CStylePropertyTextShadow>(VMTs.textShadowVmt, symbols.textShadowSymbol, true, params.horizontalOffset, params.verticalOffset, params.blurRadius, params.strength, params.color);
    }

    [[nodiscard]] cs2::CStylePropertyMargin* margin(cs2::CUILength left, cs2::CUILength top, cs2::CUILength right, cs2::CUILength bottom) const noexcept
    {
        return create<cs2::CStylePropertyMargin>(VMTs.marginVmt, symbols.marginSymbol, left, top, right, bottom);
    }

private:
    template <typename T, typename... Args>
    [[nodiscard]] T* create(const void* vmt, cs2::CStyleSymbol symbol, Args&&... args) const noexcept
    {
        if (vmt && symbol.isValid()) {
            if (const auto memory{MemAlloc::allocate(sizeof(T))})
                return new (memory) T{cs2::CStyleProperty{vmt, symbol, false}, std::forward<Args>(args)...};
        }
        return nullptr;
    }

    const StylePropertiesVMTs& VMTs;
    const StylePropertiesSymbols& symbols;
};
