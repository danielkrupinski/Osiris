#pragma once

#include <GameClasses/OffsetTypes/TopLevelWindowOffset.h>

struct TopLevelWindowDeps {
    TopLevelWindowDeps() = default;

    template <typename TopLevelWindowPatterns>
    explicit TopLevelWindowDeps(const TopLevelWindowPatterns& topLevelWindowPatterns) noexcept
        : uiScaleFactorOffset{topLevelWindowPatterns.uiScaleFactorOffset()}
    {
    }

    [[nodiscard]] static const TopLevelWindowDeps& instance() noexcept;

    UiScaleFactorOffset uiScaleFactorOffset;
};
