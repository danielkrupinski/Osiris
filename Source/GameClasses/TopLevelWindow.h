#pragma once

#include <CS2/Panorama/CTopLevelWindow.h>
#include <GameDependencies/TopLevelWindowDeps.h>

class TopLevelWindow {
public:
    explicit TopLevelWindow(cs2::CTopLevelWindow* thisptr) noexcept
        : thisptr{thisptr}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return thisptr != nullptr;
    }

    [[nodiscard]] float getUiScaleFactor() const noexcept
    {
        return TopLevelWindowDeps::instance().uiScaleFactorOffset.of(thisptr).valueOr(1.0f);
    }

private:
    cs2::CTopLevelWindow* thisptr;
};
