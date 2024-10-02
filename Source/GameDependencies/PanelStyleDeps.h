#pragma once

#include <CS2/Panorama/CPanelStyle.h>

struct PanelStyleDeps {
    PanelStyleDeps() = default;

    template <typename PanelStylePatterns>
    explicit PanelStyleDeps(const PanelStylePatterns& panelStylePatterns) noexcept
        : setProperty{panelStylePatterns.setProperty()}
    {
    }

    cs2::CPanelStyle::SetProperty* setProperty;
};
