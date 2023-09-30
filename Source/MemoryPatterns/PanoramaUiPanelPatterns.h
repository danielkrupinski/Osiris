#pragma once

#include <cstdint>

#include <GameClasses/OffsetTypes/PanoramaUiPanelOffset.h>

using ChildPanelsVectorOffset = PanoramaUiPanelOffset<cs2::CUIPanel::childrenVector, std::int8_t>;
using PanelClassesVectorOffset = PanoramaUiPanelOffset<cs2::CUIPanel::classesVector, std::int32_t>;
using PanelStyleOffset = PanoramaUiPanelOffset<cs2::CPanelStyle, std::int8_t>;

struct PanoramaUiPanelPatterns {
    [[nodiscard]] static std::int32_t* setParent() noexcept;
    [[nodiscard]] static std::int32_t* setVisible() noexcept;
    [[nodiscard]] static std::int32_t* findChildInLayoutFile() noexcept;
    [[nodiscard]] static std::int32_t* getAttributeString() noexcept;
    [[nodiscard]] static std::int32_t* setAttributeString() noexcept;

    [[nodiscard]] static ChildPanelsVectorOffset childPanelsVectorOffset() noexcept;
    [[nodiscard]] static PanelClassesVectorOffset classesVectorOffset() noexcept;
    [[nodiscard]] static PanelStyleOffset panelStyleOffset() noexcept;
};
