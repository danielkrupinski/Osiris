#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/OffsetTypes/PanoramaUiPanelOffset.h>
#include <Utils/FieldOffset.h>

struct PanoramaUiPanelImpl {
    template <typename PanoramaUiPanelPatterns>
    explicit PanoramaUiPanelImpl(const PanoramaUiPanelPatterns& panoramaUiPanelPatterns) noexcept
        : setParent{panoramaUiPanelPatterns.setParent()}
        , setVisible{panoramaUiPanelPatterns.setVisible()}
        , getAttributeString{panoramaUiPanelPatterns.getAttributeString()}
        , setAttributeString{panoramaUiPanelPatterns.setAttributeString()}
        , childPanels{panoramaUiPanelPatterns.childPanelsVectorOffset()}
        , classes{panoramaUiPanelPatterns.classesVectorOffset()}
        , panelStyle{panoramaUiPanelPatterns.panelStyleOffset()}
        , parentWindowOffset{panoramaUiPanelPatterns.parentWindowOffset()}
        , offsetToPanelId{panoramaUiPanelPatterns.offsetToPanelId()}
        , offsetToPanelFlags{panoramaUiPanelPatterns.offsetToPanelFlags()}
    {
    }

    [[nodiscard]] static const PanoramaUiPanelImpl& instance() noexcept;

    template <typename T>
    using Offset = FieldOffset<const void, T, std::int32_t>;

    Offset<cs2::CUIPanel::setParent> setParent;
    Offset<cs2::CUIPanel::setVisible> setVisible;
    Offset<cs2::CUIPanel::getAttributeString> getAttributeString;
    Offset<cs2::CUIPanel::setAttributeString> setAttributeString;

    ChildPanelsVectorOffset childPanels;
    PanelClassesVectorOffset classes;
    PanelStyleOffset panelStyle;
    ParentWindowOffset parentWindowOffset;
    OffsetToPanelId offsetToPanelId;
    OffsetToPanelFlags offsetToPanelFlags;
};
