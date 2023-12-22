#pragma once

#include <cstdint>

#include <GameClasses/OffsetTypes/PanoramaUiPanelOffset.h>

using ChildPanelsVectorOffset = PanoramaUiPanelOffset<cs2::CUIPanel::childrenVector, std::int8_t>;
using PanelClassesVectorOffset = PanoramaUiPanelOffset<cs2::CUIPanel::classesVector, std::int32_t>;
using PanelStyleOffset = PanoramaUiPanelOffset<cs2::CPanelStyle, std::int8_t>;

struct PanoramaUiPanelPatterns {
    [[nodiscard]] std::int32_t* setParent() const noexcept;
    [[nodiscard]] std::int32_t* setVisible() const noexcept;
    [[nodiscard]] std::int32_t* findChildInLayoutFile() const noexcept;
    [[nodiscard]] std::int32_t* getAttributeString() const noexcept;
    [[nodiscard]] std::int32_t* setAttributeString() const noexcept;

    [[nodiscard]] ChildPanelsVectorOffset childPanelsVectorOffset() const noexcept;
    [[nodiscard]] PanelClassesVectorOffset classesVectorOffset() const noexcept;
    [[nodiscard]] PanelStyleOffset panelStyleOffset() const noexcept;
};
