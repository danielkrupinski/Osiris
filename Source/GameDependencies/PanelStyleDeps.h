#pragma once

#include <CS2/Panorama/CPanelStyle.h>

struct PanelStyleDeps {
    PanelStyleDeps() = default;

    template <typename PanelStylePatterns>
    explicit PanelStyleDeps(const PanelStylePatterns& panelStylePatterns) noexcept
        : setProperty{panelStylePatterns.setProperty()}
    {
    }

    [[nodiscard]] static const PanelStyleDeps& instance() noexcept;

    cs2::CPanelStyle::SetProperty* setProperty;
};
