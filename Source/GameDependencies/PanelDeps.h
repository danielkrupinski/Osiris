#pragma once

#include <CS2/Panorama/CPanel2D.h>

struct PanelDeps {
    PanelDeps() = default;

    template <typename PanelPatterns>
    explicit PanelDeps(const PanelPatterns& panelPatterns) noexcept
        : create{panelPatterns.create()}
    {
    }

    cs2::CPanel2D::Create* create;
};
