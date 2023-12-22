#pragma once

#include <MemoryPatterns/PanelStylePatterns.h>

struct PanelStyleImpl {
    explicit PanelStyleImpl(const PanelStylePatterns& panelStylePatterns) noexcept
        : setProperty{panelStylePatterns.setProperty()}
    {
    }

    [[nodiscard]] static const PanelStyleImpl& instance() noexcept;

    cs2::CPanelStyle::SetProperty* setProperty;
};
