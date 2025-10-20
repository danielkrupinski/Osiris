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

    [[nodiscard]] cs2::CStylePropertyForegroundColor* foregroundColor(cs2::Color color) const noexcept
    {
        const auto foregroundColor = create2<cs2::CStylePropertyForegroundColor>();
        if (foregroundColor) {
            foregroundColor->numberOfFillBrushes = 1;
            foregroundColor->growSize = 1;
            foregroundColor->fillBrush.type = cs2::k_EStrokeTypeFillColor;
            foregroundColor->fillBrush.fillColor = color;
            foregroundColor->fillBrush.opacity = 1.0f;
        }
        return foregroundColor;
    }

    [[nodiscard]] cs2::CStylePropertyBackgroundColor* backgroundColor(cs2::Color color) const noexcept
    {
        const auto backgroundColor = create2<cs2::CStylePropertyBackgroundColor>();
        if (backgroundColor) {
            backgroundColor->numberOfFillBrushes = 1;
            backgroundColor->growSize = 1;
            backgroundColor->fillBrush.type = cs2::k_EStrokeTypeFillColor;
            backgroundColor->fillBrush.fillColor = color;
            backgroundColor->fillBrush.opacity = 1.0f;
            backgroundColor->opacity = -1.0f;
        }
        return backgroundColor;
    }

    [[nodiscard]] cs2::CStylePropertyTransform3D* transform3D(std::span<cs2::CTransform3D*> transforms) const noexcept
    {
        const auto transform3D = create2<cs2::CStylePropertyTransform3D>();
        if (transform3D) {
            transform3D->transforms.memory = static_cast<cs2::CTransform3D**>(hookContext.template make<MemAlloc>().allocate(sizeof(cs2::CTransform3D*) * transforms.size()));
            if (transform3D->transforms.memory) {
                std::memcpy(transform3D->transforms.memory, transforms.data(), transforms.size() * sizeof(cs2::CTransform3D*));
                transform3D->transforms.allocationCount = static_cast<int>(transforms.size());
                transform3D->transforms.size = static_cast<int>(transforms.size());
            }
            transform3D->transforms.growSize = 1;
            transform3D->cachedParentWidth = 0.0f;
            transform3D->cachedParentHeight = 0.0f;
            transform3D->dirty = true;
            transform3D->matrix = cs2::VMatrix::identity();
            transform3D->interpolated = false;
            transform3D->fullySet = true;
        }
        return transform3D;
    }

    [[nodiscard]] cs2::CStylePropertyBorder* border(cs2::CUILength width, cs2::Color color) const noexcept
    {
        const auto styleProperty = create2<cs2::CStylePropertyBorder>();
        if (styleProperty) {
            for (int i = 0; i < 4; ++i) {
                styleProperty->m_rgBorderStyle[i] = cs2::k_EBorderStyleSolid;
                styleProperty->m_rgBorderWidth[i] = width;
                styleProperty->m_rgColorsSet[i] = true;
                styleProperty->m_rgBorderColor[i] = color;
            }
        }
        return styleProperty;
    }

    [[nodiscard]] cs2::CStylePropertyBorderRadius* borderRadius(cs2::CUILength radius) const noexcept
    {
        const auto styleProperty = create2<cs2::CStylePropertyBorderRadius>();
        if (styleProperty) {
            for (auto& corner : styleProperty->m_rgCornerRaddi) {
                corner.horizontalRadius = radius;
                corner.verticalRadius = radius;
            }
        }
        return styleProperty;
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

    template <typename T>
    [[nodiscard]] T* create2() const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<T>();
        const auto symbol = symbolsAndVMTs.getSymbol<T>();

        if (vmt && symbol.isValid()) {
            if (const auto memory{static_cast<T*>(hookContext.template make<MemAlloc>().allocate(sizeof(T)))}) {
                memory->vmt = vmt;
                memory->m_symPropertyName = symbol;
                memory->m_bDisallowTransition = false;
                return memory;
            }
        }
        return nullptr;
    }

    HookContext& hookContext;
    const StylePropertiesSymbolsAndVMTs& symbolsAndVMTs;
};
