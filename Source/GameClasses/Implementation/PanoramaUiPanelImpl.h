#pragma once

#include <CS2/Classes/Panorama.h>
#include <MemoryPatterns/PanoramaUiPanelPatterns.h>
#include <Utils/FieldOffset.h>

struct PanoramaUiPanelImpl {
    [[nodiscard]] static const PanoramaUiPanelImpl& instance() noexcept;

    template <typename T>
    using Offset = FieldOffset<const void, T, std::int32_t>;

    Offset<cs2::CUIPanel::setParent> setParent{ PanoramaUiPanelPatterns::setParent() };
    Offset<cs2::CUIPanel::setVisible> setVisible{ PanoramaUiPanelPatterns::setVisible() };
    Offset<cs2::CUIPanel::findChildInLayoutFile> findChildInLayoutFile{ PanoramaUiPanelPatterns::findChildInLayoutFile() };
    Offset<cs2::CUIPanel::getAttributeString> getAttributeString{ PanoramaUiPanelPatterns::getAttributeString() };
    Offset<cs2::CUIPanel::setAttributeString> setAttributeString{ PanoramaUiPanelPatterns::setAttributeString() };

    ChildPanelsVectorOffset childPanels{ PanoramaUiPanelPatterns::childPanelsVectorOffset() };
    PanelClassesVectorOffset classes{ PanoramaUiPanelPatterns::classesVectorOffset() };
    PanelStyleOffset panelStyle{ PanoramaUiPanelPatterns::panelStyleOffset() };
};
