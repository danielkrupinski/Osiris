#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClasses/OffsetTypes/PanoramaUiPanelOffset.h>
#include <Utils/FieldOffset.h>

struct PanoramaUiPanelDeps {
    PanoramaUiPanelDeps() = default;

    template <typename PanoramaUiPanelPatterns>
    explicit PanoramaUiPanelDeps(const PanoramaUiPanelPatterns& panoramaUiPanelPatterns) noexcept
        : setParent{panoramaUiPanelPatterns.setParent()}
        , setVisible{panoramaUiPanelPatterns.setVisible()}
        , getAttributeString{panoramaUiPanelPatterns.getAttributeString()}
        , setAttributeString{panoramaUiPanelPatterns.setAttributeString()}
        , childPanels{panoramaUiPanelPatterns.childPanelsVectorOffset()}
        , panelStyle{panoramaUiPanelPatterns.panelStyleOffset()}
        , parentWindowOffset{panoramaUiPanelPatterns.parentWindowOffset()}
        , offsetToPanelId{panoramaUiPanelPatterns.offsetToPanelId()}
        , classes{panoramaUiPanelPatterns.classesVectorOffset()}
        , offsetToPanelFlags{panoramaUiPanelPatterns.offsetToPanelFlags()}
    {
    }

    template <typename T>
    using Offset = FieldOffset<const void, T, std::int32_t>;

    Offset<cs2::CUIPanel::setParent> setParent;
    Offset<cs2::CUIPanel::setVisible> setVisible;
    Offset<cs2::CUIPanel::getAttributeString> getAttributeString;
    Offset<cs2::CUIPanel::setAttributeString> setAttributeString;

    ChildPanelsVectorOffset childPanels;
    PanelStyleOffset panelStyle;
    ParentWindowOffset parentWindowOffset;
    OffsetToPanelId offsetToPanelId;
    PanelClassesVectorOffset classes;
    OffsetToPanelFlags offsetToPanelFlags;
};
