#pragma once

#include <string_view>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/CUtlVector.h>
#include <CS2/Panorama/CPanoramaSymbol.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/CUIPanel.h>
#include <CS2/Panorama/StyleEnums.h>
#include <CS2/Panorama/Transform3D.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>
#include <GameClient/Panorama/PanelShadowParams.h>
#include <Utils/Lvalue.h>

#include "PanoramaUiPanelContext.h"

template <typename HookContext, typename Context = PanoramaUiPanelContext<HookContext>>
struct PanoramaUiPanel {
    template <typename... Args>
        requires std::is_constructible_v<Context, Args...>
    explicit PanoramaUiPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    template <template <typename...> typename T, typename... Args>
    [[nodiscard]] decltype(auto) as(Args&&... args) const noexcept
    {
        return context.template as<T>(std::forward<Args>(args)...);
    }

    [[nodiscard]] decltype(auto) getHandle() const noexcept
    {
        return context.getHandle();
    }

    [[nodiscard]] decltype(auto) clientPanel() const noexcept
    {
        return context.clientPanel();
    }

    [[nodiscard]] std::string_view getId() const noexcept
    {
        if (auto&& id = context.getId())
            return id;
        return {};
    }
    
    void setParent(cs2::CUIPanel* parent) const noexcept
    {
        if (parent) {
            if (auto&& setParentFn = context.setParent())
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
            if (auto&& setVisibleFn = context.setVisible())
                setVisibleFn(visible);
        }
    }

    [[nodiscard]] decltype(auto) findChildInLayoutFile(const char* childId) const noexcept
    {
        auto&& childPanels = context.childPanels();

        for (auto&& childPanel : childPanels) {
            if (childPanel.getId() == childId)
                return utils::lvalue<decltype(childPanel)>(childPanel);
        }

        for (auto&& childPanel : childPanels) {
            if (!childPanel.hasOwnLayoutFile().valueOr(true)) {
                if (auto&& foundPanel = childPanel.findChildInLayoutFile(childId))
                    return utils::lvalue<decltype(foundPanel)>(foundPanel);
            }
        }

        return context.nullPanel();
    }

    [[nodiscard]] const char* getAttributeString(cs2::CPanoramaSymbol attributeName, const char* defaultValue) const noexcept
    {
        if (auto&& getAttributeStringFn = context.getAttributeString())
            return getAttributeStringFn(attributeName, defaultValue);
        return defaultValue;
    }

    void setAttributeString(cs2::CPanoramaSymbol attributeName, const char* value) const noexcept
    {
        if (auto&& setAttributeStringFn = context.setAttributeString())
            return setAttributeStringFn(attributeName, value);
    }

    [[nodiscard]] bool hasClass(cs2::CPanoramaSymbol className) const noexcept
    {
        return context.classes().hasClass(className);
    }

    [[nodiscard]] auto hasOwnLayoutFile() const noexcept
    {
        return context.hasFlag(cs2::k_EPanelFlag_HasOwnLayoutFile);
    }

    [[nodiscard]] auto isVisible() const noexcept
    {
        return context.hasFlag(cs2::k_EPanelFlag_IsVisible);
    }

    [[nodiscard]] decltype(auto) children() const noexcept
    {
        return context.childPanels();
    }

    explicit(false) operator cs2::CUIPanel*() const noexcept
    {
        return context.getRawPointer();
    }

    explicit operator bool() const noexcept
    {
        return context.getRawPointer() != nullptr;
    }

    void setOpacity(float opacity) const noexcept
    {
        context.setProperty(context.propertyFactory().opacity(opacity));
    }

    void setWidth(cs2::CUILength width) const noexcept
    {
        context.setProperty(context.propertyFactory().width(width));
    }

    void setHeight(cs2::CUILength height) const noexcept
    {
        context.setProperty(context.propertyFactory().height(height));
    }

    void setZIndex(float zIndex) const noexcept
    {
        context.setProperty(context.propertyFactory().zIndex(zIndex));
    }

    void setImageShadow(const PanelShadowParams& params) const noexcept
    {
        context.setProperty(context.propertyFactory().imageShadow(params));
    }

    void setPosition(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        context.setProperty(context.propertyFactory().position(x, y));
    }

    void setTransformOrigin(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        context.setProperty(context.propertyFactory().transformOrigin(x, y));
    }

    void setAlign(const PanelAlignmentParams& params) const noexcept
    {
        context.setProperty(context.propertyFactory().align(params));
    }

    void setWashColor(cs2::Color color) const noexcept
    {
        context.setProperty(context.propertyFactory().washColor(color));
    }

    void setFlowChildren(cs2::EFlowDirection flowDirection) const noexcept
    {
        context.setProperty(context.propertyFactory().flowChildren(flowDirection));
    }

    void setFont(const PanelFontParams& params) const noexcept
    {
        context.setProperty(context.propertyFactory().font(params));
    }

    void setTextShadow(const PanelShadowParams& params) const noexcept
    {
        context.setProperty(context.propertyFactory().textShadow(params));
    }

    void setMargin(const PanelMarginParams& params) const noexcept
    {
        context.setProperty(context.propertyFactory().margin(params));
    }

    void setMixBlendMode(cs2::EMixBlendMode mode) const noexcept
    {
        context.setProperty(context.propertyFactory().mixBlendMode(mode));
    }

    void setTextAlign(cs2::ETextAlign textAlign) const noexcept
    {
        context.setProperty(context.propertyFactory().textAlign(textAlign));
    }

    void setColor(cs2::Color color) const noexcept
    {
        context.setSimpleForegroundColor(color);
    }

    void setTransform3D(const cs2::CUtlVector<cs2::CTransform3D*>& transforms) const noexcept
    {
        context.setTransform3D(transforms);
    }

    [[nodiscard]] decltype(auto) getUiScaleFactor() const noexcept
    {
        return context.getParentWindow().getUiScaleFactor();
    }

private:
    Context context;
};
