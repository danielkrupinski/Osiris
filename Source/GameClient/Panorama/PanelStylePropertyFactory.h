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
            return cs2::CStylePropertyWidth{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_Length = width
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyOpacity> opacity(float opacity) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyOpacity>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyOpacity>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyOpacity{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .opacity = opacity
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyZIndex> zIndex(float zIndex) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyZIndex>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyZIndex>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyZIndex{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .zindex = zIndex
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyHeight> height(cs2::CUILength height) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyHeight>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyHeight>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyHeight{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_Height = height
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyImageShadow> imageShadow(const PanelShadowParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyImageShadow>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyImageShadow>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyImageShadow{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .fullySet = true,
                .horizontalOffset = params.horizontalOffset,
                .verticalOffset = params.verticalOffset,
                .blurRadius = params.blurRadius,
                .strength = params.strength,
                .color = params.color
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyPosition> position(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyPosition>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyPosition>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyPosition{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .x = x,
                .y = y,
                .z = cs2::CUILength::pixels(0)
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyTransformOrigin> transformOrigin(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyTransformOrigin>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyTransformOrigin>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyTransformOrigin{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .x = x,
                .y = y,
                .m_bParentRelative = false
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyAlign> align(const PanelAlignmentParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyAlign>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyAlign>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyAlign{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_eHorizontalAlignment = params.horizontalAlignment,
                .m_eVerticalAlignment = params.verticalAlignment
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyWashColor> washColor(cs2::Color color) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyWashColor>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyWashColor>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyWashColor{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .color = color,
                .fullySet = true
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyFlowChildren> flowChildren(cs2::EFlowDirection flowDirection) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyFlowChildren>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyFlowChildren>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyFlowChildren{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_eFlowDirection = flowDirection
            };
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
            return cs2::CStylePropertyFont{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_strFontFamily = fontFamilyString,
                .m_flFontSize = params.fontSize,
                .m_eFontStyle = cs2::k_EFontStyleUnset,
                .m_eFontWeight = params.fontWeight,
                .m_eFontStretch = cs2::k_EFontStretchUnset
            };
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyTextShadow> textShadow(const PanelShadowParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyTextShadow>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyTextShadow>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyTextShadow{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .fullySet = true, 
                .horizontalOffset = params.horizontalOffset, 
                .verticalOffset = params.verticalOffset,
                .blurRadius = params.blurRadius,
                .strength = params.strength,
                .color = params.color
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyMargin> margin(const PanelMarginParams& params) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyMargin>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyMargin>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyMargin{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_left = params.marginLeft,
                .m_top = params.marginTop,
                .m_right = params.marginRight,
                .m_bottom = params.marginBottom
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyMixBlendMode> mixBlendMode(cs2::EMixBlendMode mode) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyMixBlendMode>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyMixBlendMode>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyMixBlendMode{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_eMixBlendMode = mode,
                .m_bSet = true
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyTextAlign> textAlign(cs2::ETextAlign textAlign) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyTextAlign>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyTextAlign>();
        if (vmt && symbol.isValid())
            return cs2::CStylePropertyTextAlign{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_eAlign = textAlign
            };
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyForegroundColor> foregroundColor(cs2::Color color) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyForegroundColor>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyForegroundColor>();
        if (vmt && symbol.isValid()) {
            return cs2::CStylePropertyForegroundColor{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .fillBrushes{
                    .numberOfElements = 1,
                    .growSize = 1,
                    .element{
                        .type = cs2::k_EStrokeTypeFillColor,
                        .fillColor = color,
                        .opacity = 1.0f
                    }
                }
            };
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyBackgroundColor> backgroundColor(cs2::Color color) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyBackgroundColor>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyBackgroundColor>();
        if (vmt && symbol.isValid()) {
            return cs2::CStylePropertyBackgroundColor{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .fillBrushes{
                    .numberOfElements = 1,
                    .growSize = 1,
                    .element{
                        .type = cs2::k_EStrokeTypeFillColor,
                        .fillColor = color,
                        .opacity = 1.0f
                    },
                },
                .opacity = -1.0f
            };
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyTransform3D> transform3D(std::span<cs2::CTransform3D*> transforms) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyTransform3D>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyTransform3D>();
        if (vmt && symbol.isValid()) {
            return cs2::CStylePropertyTransform3D{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .transforms{createTransformVector(transforms)},
                .cachedParentWidth = 0.0f,
                .cachedParentHeight = 0.0f,
                .dirty = true,
                .matrix = cs2::VMatrix::identity(),
                .interpolated = false,
                .fullySet = true
            };
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyBorder> border(cs2::CUILength width, cs2::Color color) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyBorder>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyBorder>();
        if (vmt && symbol.isValid()) {
            return cs2::CStylePropertyBorder{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_rgBorderStyle{cs2::k_EBorderStyleSolid, cs2::k_EBorderStyleSolid, cs2::k_EBorderStyleSolid, cs2::k_EBorderStyleSolid},
                .m_rgBorderWidth{width, width, width, width},
                .m_rgColorsSet{true, true, true, true},
                .m_rgBorderColor{color, color, color, color}
            };
        }
        return {};
    }

    [[nodiscard]] std::optional<cs2::CStylePropertyBorderRadius> borderRadius(cs2::CUILength radius) const noexcept
    {
        const auto vmt = symbolsAndVMTs.getVmt<cs2::CStylePropertyBorderRadius>();
        const auto symbol = symbolsAndVMTs.getSymbol<cs2::CStylePropertyBorderRadius>();
        if (vmt && symbol.isValid()) {
            return cs2::CStylePropertyBorderRadius{
                .vmt = vmt,
                .m_symPropertyName = symbol,
                .m_bDisallowTransition = false,
                .m_rgCornerRaddi{
                    {.horizontalRadius = radius, .verticalRadius = radius},
                    {.horizontalRadius = radius, .verticalRadius = radius},
                    {.horizontalRadius = radius, .verticalRadius = radius},
                    {.horizontalRadius = radius, .verticalRadius = radius}
                }
            };
        }
        return {};
    }

private:
    [[nodiscard]] auto createTransformVector(std::span<cs2::CTransform3D*> transforms) const
    {
        cs2::CUtlVector<cs2::CTransform3D*> vec{};
        vec.memory = static_cast<cs2::CTransform3D**>(hookContext.template make<MemAlloc>().allocate(sizeof(cs2::CTransform3D*) * transforms.size()));
        if (vec.memory) {
            std::memcpy(vec.memory, transforms.data(), transforms.size() * sizeof(cs2::CTransform3D*));
            vec.allocationCount = static_cast<int>(transforms.size());
            vec.size = static_cast<int>(transforms.size());
        }
        vec.growSize = 1;
        return vec;
    }

    HookContext& hookContext;
    const StylePropertiesSymbolsAndVMTs& symbolsAndVMTs;
};
