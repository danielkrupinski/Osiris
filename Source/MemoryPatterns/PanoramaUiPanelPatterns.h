#pragma once

#include <cstdint>

#include <GameClasses/OffsetTypes/PanoramaUiPanelOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PanoramaUiPanelPatterns {
    [[nodiscard]] std::int32_t* setParent() const noexcept;
    [[nodiscard]] std::int32_t* setVisible() const noexcept;
    [[nodiscard]] std::int32_t* findChildInLayoutFile() const noexcept;
    [[nodiscard]] std::int32_t* getAttributeString() const noexcept;
    [[nodiscard]] std::int32_t* setAttributeString() const noexcept;

    [[nodiscard]] ChildPanelsVectorOffset childPanelsVectorOffset() const noexcept;
    [[nodiscard]] PanelClassesVectorOffset classesVectorOffset() const noexcept;
    [[nodiscard]] PanelStyleOffset panelStyleOffset() const noexcept;
    [[nodiscard]] ParentWindowOffset parentWindowOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
    const PatternFinder<PatternNotFoundLogger>& panoramaPatternFinder;
};
