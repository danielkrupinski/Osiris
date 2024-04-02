#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Panorama.h>
#include "GameClasses/Implementation/PanelStyleImpl.h"
#include "PanelStylePropertyFactory.h"

struct ImageShadowParams;

class PanelStyleSetter {
public:
    PanelStyleSetter(cs2::CPanelStyle& style, PanelStylePropertyFactory propertyFactory) noexcept
        : thisptr{&style}
        , propertyFactory{propertyFactory}
    {
    }

    void setProperty(cs2::CStyleProperty* styleProperty) const noexcept
    {
        if (!styleProperty)
            return;

        if (const auto setPropertyFn{PanelStyleImpl::instance().setProperty})
            setPropertyFn(thisptr, styleProperty, true);
    }

    void fitParent() const noexcept
    {
        setWidth(cs2::CUILength{100.0f, cs2::CUILength::k_EUILengthPercent});
        setHeight(cs2::CUILength{100.0f, cs2::CUILength::k_EUILengthPercent});
    }

    void setSimpleForegroundColor(cs2::Color color) const noexcept
    {
        // FIXME: hardcoded virtual method index
        reinterpret_cast<void(*)(cs2::CPanelStyle* thisptr, const cs2::Color* color)>((*reinterpret_cast<void(***)()>(thisptr))[54])(thisptr, &color);
    }

    void setTransform3D(const cs2::CUtlVector<cs2::CTransform3D*>& transforms) const noexcept
    {
        // FIXME: hardcoded virtual method index
        reinterpret_cast<void(*)(cs2::CPanelStyle* thisptr, const cs2::CUtlVector<cs2::CTransform3D*>* transforms)>((*reinterpret_cast<void(***)()>(thisptr))[17])(thisptr, &transforms);
    }

    void setOpacity(float opacity) const noexcept
    {
        setProperty(propertyFactory.opacity(opacity));
    }

    void setWidth(cs2::CUILength width) const noexcept
    {
        setProperty(propertyFactory.width(width));
    }

    void setHeight(cs2::CUILength height) const noexcept
    {
        setProperty(propertyFactory.height(height));
    }

    void setZIndex(float zIndex) const noexcept
    {
        setProperty(propertyFactory.zIndex(zIndex));
    }

    void setImageShadow(const PanelShadowParams& params) const noexcept
    {
        setProperty(propertyFactory.imageShadow(params));
    }

    void setPosition(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        setProperty(propertyFactory.position(x, y));
    }

    void setTransformOrigin(cs2::CUILength x, cs2::CUILength y) const noexcept
    {
        setProperty(propertyFactory.transformOrigin(x, y));
    }

    void setAlign(cs2::EHorizontalAlignment horizontalAlignment, cs2::EVerticalAlignment verticalAlignment) const noexcept
    {
        setProperty(propertyFactory.align(horizontalAlignment, verticalAlignment));
    }

    void setWashColor(cs2::Color color) const noexcept
    {
        setProperty(propertyFactory.washColor(color));
    }

    void setFlowChildren(cs2::EFlowDirection flowDirection) const noexcept
    {
        setProperty(propertyFactory.flowChildren(flowDirection));
    }

    void setFont(std::string_view fontFamily, float fontSize, cs2::EFontWeight fontWeight) const noexcept
    {
        setProperty(propertyFactory.font(fontFamily, fontSize, fontWeight));
    }

    void setTextShadow(const PanelShadowParams& params) const noexcept
    {
        setProperty(propertyFactory.textShadow(params));
    }

    void setMargin(cs2::CUILength left, cs2::CUILength top, cs2::CUILength right, cs2::CUILength bottom) const noexcept
    {
        setProperty(propertyFactory.margin(left, top, right, bottom));
    }

private:
    cs2::CPanelStyle* thisptr;
    PanelStylePropertyFactory propertyFactory;
};
