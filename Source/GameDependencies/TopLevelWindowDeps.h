#pragma once

#include <GameClasses/OffsetTypes/TopLevelWindowOffset.h>

struct TopLevelWindowDeps {
    TopLevelWindowDeps() = default;

    template <typename TopLevelWindowPatterns>
    explicit TopLevelWindowDeps(const TopLevelWindowPatterns& topLevelWindowPatterns) noexcept
        : uiScaleFactorOffset{topLevelWindowPatterns.uiScaleFactorOffset()}
    {
    }

    UiScaleFactorOffset uiScaleFactorOffset;
};
