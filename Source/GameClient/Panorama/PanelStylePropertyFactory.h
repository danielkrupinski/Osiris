#pragma once

#include <cstring>
#include <utility>

#include <CS2/Classes/CUtlString.h>
#include <CS2/Classes/Color.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>

#include <CS2/Panorama/StyleProperties.h>
#include <GameClient/MemAlloc.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>

#include "PanelShadowParams.h"
#include "StylePropertiesSymbolsAndVMTs.h"

template <typename HookContext>
struct PanelStylePropertyFactory {
    PanelStylePropertyFactory(HookContext& hookContext, const StylePropertiesSymbolsAndVMTs& symbolsAndVMTs) noexcept
        : hookContext{hookContext}
        , symbolsAndVMTs{symbolsAndVMTs}
    {
    }

    [[nodiscard]] cs2::CStylePropertyWidth* width(cs2::CUILength width) const noexcept
    {
        return create<cs2::CStylePropertyWidth>(width);
    }

    [[nodiscard]] cs2::CStylePropertyOpacity* opacity(float opacity) const noexcept
    {
        return create<cs2::CStylePropertyOpacity>(opacity);
    }

    [[nodiscard]] cs2::CStylePropertyZIndex* zIndex(float zIndex) const noexcept
    {
        return create<cs2::CStylePropertyZIndex>(zIndex);
    }

    [[nodiscard]] cs2::CStylePropertyHeight* height(cs2::CUILength height) const noexcept
    {
        return create<cs2::CStylePropertyHeight>(height);
    }

    [[nodiscard]] cs2::CStylePropertyImageShadow* imageShadow(const PanelShadowParams& params) const noexcept
    {
        return create<cs2::CStylePropertyImageShadow>(true, params.horizontalOffset, params.verticalOffset, params.blurRadius, params.strength, params.color);
    }

    [[nodiscard]] cs2::CStylePropertyPosition* position(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        return create<cs2::CStylePropertyPosition>(x, y, cs2::CUILength::pixels(0));
    }

    [[nodiscard]] cs2::CStylePropertyTransformOrigin* transformOrigin(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        return create<cs2::CStylePropertyTransformOrigin>(x, y, false);
    }

    [[nodiscard]] cs2::CStylePropertyAlign* align(const PanelAlignmentParams& params) const noexcept
    {
        return create<cs2::CStylePropertyAlign>(params.horizontalAlignment, params.verticalAlignment);
    }

    [[nodiscard]] cs2::CStylePropertyWashColor* washColor(cs2::Color color) const noexcept
    {
        return create<cs2::CStylePropertyWashColor>(color, true);
    }

    [[nodiscard]] cs2::CStylePropertyFlowChildren* flowChildren(cs2::EFlowDirection flowDirection) const noexcept
    {
        return create<cs2::CStylePropertyFlowChildren>(flowDirection);
    }

    [[nodiscard]] cs2::CStylePropertyFont* font(const PanelFontParams& params) const noexcept
    {
        cs2::CUtlString fontFamilyString{nullptr};
        if (params.fontFamily.length() > 0) {
            if ((fontFamilyString.m_pString = static_cast<char*>(hookContext.template make<MemAlloc>().allocate(params.fontFamily.length() + 1))) != nullptr) {
                std::memcpy(fontFamilyString.m_pString, params.fontFamily.data(), params.fontFamily.length());
                fontFamilyString.m_pString[params.fontFamily.length()] = '\0';
            }
        }
        return create<cs2::CStylePropertyFont>(fontFamilyString, params.fontSize, cs2::k_EFontStyleUnset, params.fontWeight, cs2::k_EFontStretchUnset);
    }

    [[nodiscard]] cs2::CStylePropertyTextShadow* textShadow(const PanelShadowParams& params) const noexcept
    {
        return create<cs2::CStylePropertyTextShadow>(true, params.horizontalOffset, params.verticalOffset, params.blurRadius, params.strength, params.color);
    }

    [[nodiscard]] cs2::CStylePropertyMargin* margin(const PanelMarginParams& params) const noexcept
    {
        return create<cs2::CStylePropertyMargin>(params.marginLeft, params.marginTop, params.marginRight, params.marginBottom);
    }

    [[nodiscard]] cs2::CStylePropertyMixBlendMode* mixBlendMode(cs2::EMixBlendMode mode) const noexcept
    {
        return create<cs2::CStylePropertyMixBlendMode>(mode, true);
    }

    [[nodiscard]] cs2::CStylePropertyTextAlign* textAlign(cs2::ETextAlign textAlign) const noexcept
    {
        return create<cs2::CStylePropertyTextAlign>(textAlign);
    }

private:
    template <typename T, typename... Args>
    [[nodiscard]] T* create(Args&&... args) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<T>();
        const auto symbol = symbolsAndVMTs.getSymbol<T>();

        if (vmt && symbol.isValid()) {
            if (const auto memory{hookContext.template make<MemAlloc>().allocate(sizeof(T))})
                return new (memory) T{cs2::CStyleProperty{vmt, symbol, false}, std::forward<Args>(args)...};
        }
        return nullptr;
    }

    HookContext& hookContext;
    const StylePropertiesSymbolsAndVMTs& symbolsAndVMTs;
};
