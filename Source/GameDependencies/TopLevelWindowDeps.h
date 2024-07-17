#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/OffsetTypes/TopLevelWindowOffset.h>

struct TopLevelWindowDeps {
    template <typename TopLevelWindowPatterns>
    explicit TopLevelWindowDeps(const TopLevelWindowPatterns& topLevelWindowPatterns) noexcept
        : uiScaleFactorOffset{topLevelWindowPatterns.uiScaleFactorOffset()}
    {
    }

    [[nodiscard]] static const TopLevelWindowDeps& instance() noexcept;

    UiScaleFactorOffset uiScaleFactorOffset;
};
