#pragma once

#include <CS2/Classes/Panorama.h>
#include <MemoryPatterns/TopLevelWindowPatterns.h>

struct TopLevelWindowImpl {
    explicit TopLevelWindowImpl(const TopLevelWindowPatterns& topLevelWindowPatterns) noexcept
        : uiScaleFactorOffset{topLevelWindowPatterns.uiScaleFactorOffset()}
    {
    }

    [[nodiscard]] static const TopLevelWindowImpl& instance() noexcept;

    UiScaleFactorOffset uiScaleFactorOffset;
};
