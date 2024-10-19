#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClasses/OffsetTypes/PanoramaUiPanelOffset.h>
#include <Utils/FieldOffset.h>

struct PanoramaUiPanelDeps {
    PanoramaUiPanelDeps() = default;

    template <typename PanoramaUiPanelPatterns>
    explicit PanoramaUiPanelDeps(const PanoramaUiPanelPatterns& panoramaUiPanelPatterns) noexcept
        : childPanels{panoramaUiPanelPatterns.childPanelsVectorOffset()}
        , panelStyle{panoramaUiPanelPatterns.panelStyleOffset()}
        , parentWindowOffset{panoramaUiPanelPatterns.parentWindowOffset()}
        , offsetToPanelId{panoramaUiPanelPatterns.offsetToPanelId()}
        , classes{panoramaUiPanelPatterns.classesVectorOffset()}
        , offsetToPanelFlags{panoramaUiPanelPatterns.offsetToPanelFlags()}
    {
    }

    ChildPanelsVectorOffset childPanels;
    PanelStyleOffset panelStyle;
    ParentWindowOffset parentWindowOffset;
    OffsetToPanelId offsetToPanelId;
    PanelClassesVectorOffset classes;
    OffsetToPanelFlags offsetToPanelFlags;
};
