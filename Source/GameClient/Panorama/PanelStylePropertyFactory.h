#pragma once

#include <cstring>
#include <optional>
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

    [[nodiscard]] std::optional<cs2::CStylePropertyWidth> width(cs2::CUILength width) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyWidth>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyWidth>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyWidth{cs2::CStyleProperty{vmt, symbol, false}, width};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyOpacity> opacity(float opacity) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyOpacity>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyOpacity>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyOpacity{cs2::CStyleProperty{vmt, symbol, false}, opacity};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyZIndex> zIndex(float zIndex) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyZIndex>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyZIndex>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyZIndex{cs2::CStyleProperty{vmt, symbol, false}, zIndex};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyHeight> height(cs2::CUILength height) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyHeight>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyHeight>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyHeight{cs2::CStyleProperty{vmt, symbol, false}, height};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyImageShadow> imageShadow(const PanelShadowParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyImageShadow>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyImageShadow>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyImageShadow{cs2::CStyleProperty{vmt, symbol, false}, true, params.horizontalOffset, params.verticalOffset, params.blurRadius, params.strength, params.color};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyPosition> position(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyPosition>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyPosition>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyPosition{cs2::CStyleProperty{vmt, symbol, false}, x, y, cs2::CUILength::pixels(0)};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyTransformOrigin> transformOrigin(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyTransformOrigin>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyTransformOrigin>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyTransformOrigin{cs2::CStyleProperty{vmt, symbol, false}, x, y, false};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyAlign> align(const PanelAlignmentParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyAlign>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyAlign>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyAlign{cs2::CStyleProperty{vmt, symbol, false}, params.horizontalAlignment, params.verticalAlignment};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyWashColor> washColor(cs2::Color color) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyWashColor>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyWashColor>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyWashColor{cs2::CStyleProperty{vmt, symbol, false}, color, true};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyFlowChildren> flowChildren(cs2::EFlowDirection flowDirection) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyFlowChildren>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyFlowChildren>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyFlowChildren{cs2::CStyleProperty{vmt, symbol, false}, flowDirection};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyFont> font(const PanelFontParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyFont>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyFont>();
        if (vmt && symbol.isValid()) {
            cs2::CUtlString fontFamilyString{nullptr};
            if (params.fontFamily.length() > 0) {
                if ((fontFamilyString.m_pString = static_cast<char*>(hookContext.template make<MemAlloc>().allocate(params.fontFamily.length() + 1))) != nullptr) {
                    std::memcpy(fontFamilyString.m_pString, params.fontFamily.data(), params.fontFamily.length());
                    fontFamilyString.m_pString[params.fontFamily.length()] = '\0';
                }
            }
            return cs2::CStylePropertyFont{cs2::CStyleProperty{vmt, symbol, false}, fontFamilyString, params.fontSize, cs2::k_EFontStyleUnset, params.fontWeight, cs2::k_EFontStretchUnset};
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyTextShadow> textShadow(const PanelShadowParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyTextShadow>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyTextShadow>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyTextShadow{cs2::CStyleProperty{vmt, symbol, false}, true, params.horizontalOffset, params.verticalOffset, params.blurRadius, params.strength, params.color};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyMargin> margin(const PanelMarginParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyMargin>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyMargin>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyMargin{cs2::CStyleProperty{vmt, symbol, false}, params.marginLeft, params.marginTop, params.marginRight, params.marginBottom};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyMixBlendMode> mixBlendMode(cs2::EMixBlendMode mode) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyMixBlendMode>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyMixBlendMode>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyMixBlendMode{cs2::CStyleProperty{vmt, symbol, false}, mode, true};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyTextAlign> textAlign(cs2::ETextAlign textAlign) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyTextAlign>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyTextAlign>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyTextAlign{cs2::CStyleProperty{vmt, symbol, false}, textAlign};
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyForegroundColor> foregroundColor(cs2::Color color) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyForegroundColor>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyForegroundColor>();
        if (vmt && symbol.isValid()) {
#if defined(CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif
            auto foregroundColor = cs2::CStylePropertyForegroundColor{cs2::CStyleProperty{vmt, symbol, false}};
#if defined(CLANG)
#pragma clang diagnostic pop
#endif
            foregroundColor.numberOfFillBrushes = 1;
            foregroundColor.growSize = 1;
            foregroundColor.fillBrush.type = cs2::k_EStrokeTypeFillColor;
            foregroundColor.fillBrush.fillColor = color;
            foregroundColor.fillBrush.opacity = 1.0f;
            return foregroundColor;
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyBackgroundColor> backgroundColor(cs2::Color color) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyBackgroundColor>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyBackgroundColor>();
        if (vmt && symbol.isValid()) {
#if defined(CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
            auto backgroundColor = cs2::CStylePropertyBackgroundColor{cs2::CStyleProperty{vmt, symbol, false}};
#if defined(CLANG)
#pragma clang diagnostic pop
#endif
            backgroundColor.numberOfFillBrushes = 1;
            backgroundColor.growSize = 1;
            backgroundColor.fillBrush.type = cs2::k_EStrokeTypeFillColor;
            backgroundColor.fillBrush.fillColor = color;
            backgroundColor.fillBrush.opacity = 1.0f;
            backgroundColor.opacity = -1.0f;
            return backgroundColor;
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyTransform3D> transform3D(std::span<cs2::CTransform3D*> transforms) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyTransform3D>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyTransform3D>();
        if (vmt && symbol.isValid()) {
#if defined(CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif
            auto transform3D = cs2::CStylePropertyTransform3D{cs2::CStyleProperty{vmt, symbol, false}};
#if defined(CLANG)
#pragma clang diagnostic pop
#endif
            transform3D.transforms.memory = static_cast<cs2::CTransform3D**>(hookContext.template make<MemAlloc>().allocate(sizeof(cs2::CTransform3D*) * transforms.size()));
            if (transform3D.transforms.memory) {
                std::memcpy(transform3D.transforms.memory, transforms.data(), transforms.size() * sizeof(cs2::CTransform3D*));
                transform3D.transforms.allocationCount = static_cast<int>(transforms.size());
                transform3D.transforms.size = static_cast<int>(transforms.size());
            }
            transform3D.transforms.growSize = 1;
            transform3D.cachedParentWidth = 0.0f;
            transform3D.cachedParentHeight = 0.0f;
            transform3D.dirty = true;
            transform3D.matrix = cs2::VMatrix::identity();
            transform3D.interpolated = false;
            transform3D.fullySet = true;
            return transform3D;
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyBorder> border(cs2::CUILength width, cs2::Color color) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyBorder>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyBorder>();
        if (vmt && symbol.isValid()) {
            return cs2::CStylePropertyBorder{
                cs2::CStyleProperty{vmt, symbol, false},
                {cs2::k_EBorderStyleSolid, cs2::k_EBorderStyleSolid, cs2::k_EBorderStyleSolid, cs2::k_EBorderStyleSolid},
                {width, width, width, width},
                {true, true, true, true},
                {color, color, color, color}
            };
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyBorderRadius> borderRadius(cs2::CUILength radius) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyBorderRadius>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyBorderRadius>();
        if (vmt && symbol.isValid()) {
#if defined(CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif
            auto styleProperty = cs2::CStylePropertyBorderRadius{cs2::CStyleProperty{vmt, symbol, false}};
#if defined(CLANG)
#pragma clang diagnostic pop
#endif
            for (auto& corner : styleProperty.m_rgCornerRaddi) {
                corner.horizontalRadius = radius;
                corner.verticalRadius = radius;
            }
            return styleProperty;
        }
        return {};
    }

private:
    HookContext& hookContext;
    const StylePropertiesSymbolsAndVMTs& symbolsAndVMTs;
};
