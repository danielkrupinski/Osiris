#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Panorama.h>
#include "GameClasses/Implementation/PanelStyleImpl.h"
#include "PanelStylePropertyFactory.h"

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
        // FIXME: hardcoded virtual method index
        reinterpret_cast<void(*)(cs2::CPanelStyle* thisptr, cs2::CUILength height)>((*reinterpret_cast<void(***)()>(thisptr))[71])(thisptr, height);
    }

    void setZIndex(float zIndex) const noexcept
    {
        // FIXME: hardcoded virtual method index
        reinterpret_cast<void(*)(cs2::CPanelStyle* thisptr, float zIndex)>((*reinterpret_cast<void(***)()>(thisptr))[14])(thisptr, zIndex);
    }

private:
    cs2::CPanelStyle* thisptr;
    PanelStylePropertyFactory propertyFactory;
};
