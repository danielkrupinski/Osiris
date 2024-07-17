#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameDependencies/PanoramaLabelDeps.h>

struct PanoramaLabel {
    explicit PanoramaLabel(cs2::CLabel* thisptr) noexcept
        : thisptr{thisptr}
    {
    }

    explicit PanoramaLabel(cs2::CLabel* thisptr, GameDependencies* gameDependencies) noexcept
        : thisptr{thisptr}
        , gameDependencies{gameDependencies}
    {
    }

    void setText(const char* value) const noexcept
    {
        setTextInternal(value, 0, true);
    }

    void setTextInternal(const char* value, int textType, bool trustedSource) const noexcept
    {
        if (thisptr && PanoramaLabelDeps::instance().setTextInternal)
            PanoramaLabelDeps::instance().setTextInternal(thisptr, value, textType, trustedSource);
    }

    void setColor(cs2::Color color) const noexcept
    {
        if (thisptr) {
            if (const auto style{PanoramaUiPanel{thisptr->uiPanel}.getStyle()})
                gameDependencies->panelConfigurator().panelStyle(*style).setSimpleForegroundColor(color);
        }
    }

private:
    cs2::CLabel* thisptr;
    GameDependencies* gameDependencies{nullptr};
};
