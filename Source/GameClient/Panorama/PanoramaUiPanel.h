#pragma once

#include <string_view>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/CUtlVector.h>
#include <CS2/Panorama/CPanelStyle.h>
#include <CS2/Panorama/CPanoramaSymbol.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/CUIPanel.h>
#include <CS2/Panorama/StyleEnums.h>
#include <CS2/Panorama/StyleProperties.h>
#include <CS2/Panorama/Transform3D.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>
#include <GameClient/Panorama/PanelShadowParams.h>
#include <MemoryPatterns/PatternTypes/PanelStylePatternTypes.h>
#include <MemoryPatterns/PatternTypes/UiPanelPatternTypes.h>
#include <Utils/Lvalue.h>

#include "ClientPanel.h"
#include "PanelStylePropertyFactory.h"
#include "PanoramaUiPanelChildPanels.h"
#include "PanoramaUiPanelClasses.h"
#include "PanoramaUiPanelMethodInvoker.h"
#include "TopLevelWindow.h"

template <typename HookContext>
class PanoramaUiPanel {
public:
    PanoramaUiPanel(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : hookContext{hookContext}
        , panel{panel}
    {
    }

    template <template <typename...> typename T, typename... Args>
    [[nodiscard]] decltype(auto) as(Args&&... args) const noexcept
    {
        return hookContext.template make<T>(panel, std::forward<Args>(args)...);
    }

    [[nodiscard]] decltype(auto) getHandle() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToPanelHandle>().of(panel).valueOr(cs2::PanelHandle{});
    }

    [[nodiscard]] decltype(auto) clientPanel() const noexcept
    {
        return hookContext.template make<ClientPanel>(clientPanelPointer());
    }

    void setParent(cs2::CUIPanel* parent) const noexcept
    {
        if (parent) {
            if (auto&& setParentFn = setParent())
                setParentFn(parent);
        }
    }

    void fitParent() const noexcept
    {
        setWidth(cs2::CUILength{100.0f, cs2::CUILength::k_EUILengthPercent});
        setHeight(cs2::CUILength{100.0f, cs2::CUILength::k_EUILengthPercent});
    }

    void show() const noexcept
    {
        setVisible(true);
    }

    void hide() const noexcept
    {
        setVisible(false);
    }

    void setVisible(bool visible) const noexcept
    {
        if (isVisible() != visible) {
            if (auto&& setVisibleFn = setVisible())
                setVisibleFn(visible);
        }
    }

    [[nodiscard]] decltype(auto) findChildInLayoutFile(const char* childId) const noexcept
    {
        auto&& childPanels = children();

        for (auto&& childPanel : childPanels) {
            if (std::strcmp(childPanel.getId(), childId) == 0)
                return utils::lvalue<decltype(childPanel)>(childPanel);
        }

        for (auto&& childPanel : childPanels) {
            if (!childPanel.hasOwnLayoutFile().valueOr(true)) {
                if (auto&& foundPanel = childPanel.findChildInLayoutFile(childId))
                    return utils::lvalue<decltype(foundPanel)>(foundPanel);
            }
        }

        return hookContext.template make<PanoramaUiPanel<HookContext>>(nullptr);
    }

    [[nodiscard]] const char* getAttributeString(cs2::CPanoramaSymbol attributeName, const char* defaultValue) const noexcept
    {
        if (auto&& getAttributeStringFn = getAttributeString())
            return getAttributeStringFn(attributeName, defaultValue);
        return defaultValue;
    }

    void setAttributeString(cs2::CPanoramaSymbol attributeName, const char* value) const noexcept
    {
        if (auto&& setAttributeStringFn = setAttributeString())
            return setAttributeStringFn(attributeName, value);
    }

    [[nodiscard]] bool hasClass(cs2::CPanoramaSymbol className) const noexcept
    {
        return classes().hasClass(className);
    }

    [[nodiscard]] auto hasOwnLayoutFile() const noexcept
    {
        return hasFlag(cs2::k_EPanelFlag_HasOwnLayoutFile);
    }

    [[nodiscard]] auto isVisible() const noexcept
    {
        return hasFlag(cs2::k_EPanelFlag_IsVisible);
    }

    [[nodiscard]] decltype(auto) children() const noexcept
    {
        return PanoramaUiPanelChildPanels{hookContext, hookContext.patternSearchResults().template get<ChildPanelsVectorOffset>().of(panel).get()};
    }

    explicit(false) operator cs2::CUIPanel*() const noexcept
    {
        return panel;
    }

    explicit operator bool() const noexcept
    {
        return panel != nullptr;
    }

    void setOpacity(float opacity) const noexcept
    {
        setStyleProperty(propertyFactory().opacity(opacity));
    }

    void setWidth(cs2::CUILength width) const noexcept
    {
        setStyleProperty(propertyFactory().width(width));
    }

    void setHeight(cs2::CUILength height) const noexcept
    {
        setStyleProperty(propertyFactory().height(height));
    }

    void setZIndex(float zIndex) const noexcept
    {
        setStyleProperty(propertyFactory().zIndex(zIndex));
    }

    void setImageShadow(const PanelShadowParams& params) const noexcept
    {
        setStyleProperty(propertyFactory().imageShadow(params));
    }

    void setPosition(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        setStyleProperty(propertyFactory().position(x, y));
    }

    void setTransformOrigin(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        setStyleProperty(propertyFactory().transformOrigin(x, y));
    }

    void setAlign(const PanelAlignmentParams& params) const noexcept
    {
        setStyleProperty(propertyFactory().align(params));
    }

    void setWashColor(cs2::Color color) const noexcept
    {
        setStyleProperty(propertyFactory().washColor(color));
    }

    void setFlowChildren(cs2::EFlowDirection flowDirection) const noexcept
    {
        setStyleProperty(propertyFactory().flowChildren(flowDirection));
    }

    void setFont(const PanelFontParams& params) const noexcept
    {
        setStyleProperty(propertyFactory().font(params));
    }

    void setTextShadow(const PanelShadowParams& params) const noexcept
    {
        setStyleProperty(propertyFactory().textShadow(params));
    }

    void setMargin(const PanelMarginParams& params) const noexcept
    {
        setStyleProperty(propertyFactory().margin(params));
    }

    void setMixBlendMode(cs2::EMixBlendMode mode) const noexcept
    {
        setStyleProperty(propertyFactory().mixBlendMode(mode));
    }

    void setTextAlign(cs2::ETextAlign textAlign) const noexcept
    {
        setStyleProperty(propertyFactory().textAlign(textAlign));
    }

    void setColor(cs2::Color color) const noexcept
    {
        setStyleProperty(propertyFactory().foregroundColor(color));
    }
    
    void setBackgroundColor(cs2::Color color) const noexcept
    {
        setStyleProperty(propertyFactory().backgroundColor(color));
    }

    void setTransform3D(std::span<cs2::CTransform3D*> transforms) const noexcept
    {
        setStyleProperty(propertyFactory().transform3D(transforms));
    }

    void setBorder(cs2::CUILength width, cs2::Color color) const noexcept
    {
        setStyleProperty(propertyFactory().border(width, color));
    }

    void setBorderRadius(cs2::CUILength radius) const noexcept
    {
        setStyleProperty(propertyFactory().borderRadius(radius));
    }

    [[nodiscard]] decltype(auto) getUiScaleFactor() const noexcept
    {
        return getParentWindow().getUiScaleFactor();
    }

private:
    template <typename StyleProperty>
    void setStyleProperty(std::optional<StyleProperty> styleProperty) const
    {
        if (styleProperty.has_value())
            setProperty(&*styleProperty);
    }

    void setProperty(cs2::CStyleProperty* styleProperty) const noexcept
    {
        if (!styleProperty)
            return;

        const auto style = getStyle();
        if (!style)
            return;

        if (const auto setPropertyFn{hookContext.patternSearchResults().template get<SetPanelStylePropertyFunctionPointer>()})
            setPropertyFn(style, styleProperty, true);
    }

    [[nodiscard]] PanoramaUiPanelClasses classes() const noexcept
    {
        return PanoramaUiPanelClasses{hookContext.patternSearchResults().template get<PanelClassesVectorOffset>().of(panel).get()};
    }

    [[nodiscard]] decltype(auto) getParentWindow() const noexcept
    {
        return hookContext.template make<TopLevelWindow>(hookContext.patternSearchResults().template get<ParentWindowOffset>().of(panel).valueOr(nullptr));
    }

    [[nodiscard]] Optional<bool> hasFlag(cs2::EPanelFlag flag) const noexcept
    {
        return (hookContext.patternSearchResults().template get<OffsetToPanelFlags>().of(panel).toOptional() & flag) != 0;
    }

    [[nodiscard]] auto setParent() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, hookContext.patternSearchResults().template get<SetParentFunctionOffset>()};
    }

    [[nodiscard]] auto setVisible() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, hookContext.patternSearchResults().template get<SetVisibleFunctionOffset>()};
    }

    [[nodiscard]] auto getAttributeString() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, hookContext.patternSearchResults().template get<GetAttributeStringFunctionOffset>()};
    }

    [[nodiscard]] auto setAttributeString() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, hookContext.patternSearchResults().template get<SetAttributeStringFunctionOffset>()};
    }

    [[nodiscard]] auto propertyFactory() const noexcept
    {
        return hookContext.template make<PanelStylePropertyFactory>(hookContext.stylePropertySymbolsAndVMTs());
    }

    [[nodiscard]] cs2::CPanel2D* clientPanelPointer() const noexcept
    {
        if (panel)
            return panel->clientPanel;
        return nullptr;
    }

    [[nodiscard]] cs2::CPanelStyle* getStyle() const noexcept
    {
        return hookContext.patternSearchResults().template get<PanelStyleOffset>().of(panel).get();
    }

    [[nodiscard]] const char* getId() const noexcept
    {
        if (const auto id = hookContext.patternSearchResults().template get<OffsetToPanelId>().of(panel).get(); id && id->m_pString)
            return id->m_pString;
        return "";
    }

    HookContext& hookContext;
    cs2::CUIPanel* panel;
};
