#pragma once

#include <CS2/Classes/Panorama.h>

struct PanelImpl {
    template <typename PanelPatterns>
    explicit PanelImpl(const PanelPatterns& panelPatterns) noexcept
        : create{panelPatterns.create()}
    {
    }

    [[nodiscard]] static const PanelImpl& instance() noexcept;

    cs2::CPanel2D::Create* create;
};
