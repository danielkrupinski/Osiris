#pragma once

#include <MemoryPatterns/PanelPatterns.h>

struct PanelImpl {
    explicit PanelImpl(const PanelPatterns& panelPatterns) noexcept
        : create{panelPatterns.create()}
    {
    }

    [[nodiscard]] static const PanelImpl& instance() noexcept;

    cs2::CPanel2D::Create* create;
};
