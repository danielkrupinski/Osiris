#pragma once

#include <CS2/Classes/Panorama.h>

struct PanelStyleImpl {
    template <typename PanelStylePatterns>
    explicit PanelStyleImpl(const PanelStylePatterns& panelStylePatterns) noexcept
        : setProperty{panelStylePatterns.setProperty()}
    {
    }

    [[nodiscard]] static const PanelStyleImpl& instance() noexcept;

    cs2::CPanelStyle::SetProperty* setProperty;
};
