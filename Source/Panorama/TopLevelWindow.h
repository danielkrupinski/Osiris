#pragma once

#include <CS2/Panorama/CTopLevelWindow.h>
#include <MemoryPatterns/PatternTypes/TopLevelWindowPatternTypes.h>

template <typename HookContext>
class TopLevelWindow {
public:
    explicit TopLevelWindow(HookContext& hookContext, cs2::CTopLevelWindow* topLevelWindow) noexcept
        : hookContext{hookContext}
        , topLevelWindow{topLevelWindow}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return topLevelWindow != nullptr;
    }

    [[nodiscard]] auto getUiScaleFactor() const noexcept
    {
        return hookContext.panoramaPatternSearchResults().template get<UiScaleFactorOffset>().of(topLevelWindow).toOptional();
    }

private:
    HookContext& hookContext;
    cs2::CTopLevelWindow* topLevelWindow;
};
