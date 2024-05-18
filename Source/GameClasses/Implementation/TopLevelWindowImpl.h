#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/OffsetTypes/TopLevelWindowOffset.h>

struct TopLevelWindowImpl {
    template <typename TopLevelWindowPatterns>
    explicit TopLevelWindowImpl(const TopLevelWindowPatterns& topLevelWindowPatterns) noexcept
        : uiScaleFactorOffset{topLevelWindowPatterns.uiScaleFactorOffset()}
    {
    }

    [[nodiscard]] static const TopLevelWindowImpl& instance() noexcept;

    UiScaleFactorOffset uiScaleFactorOffset;
};
