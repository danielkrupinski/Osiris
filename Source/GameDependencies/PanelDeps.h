#pragma once

#include <CS2/Panorama/CPanel2D.h>

struct PanelDeps {
    template <typename PanelPatterns>
    explicit PanelDeps(const PanelPatterns& panelPatterns) noexcept
        : create{panelPatterns.create()}
    {
    }

    [[nodiscard]] static const PanelDeps& instance() noexcept;

    cs2::CPanel2D::Create* create;
};
