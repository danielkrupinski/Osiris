#pragma once

#include <MemoryPatterns/PanelPatterns.h>

struct PanelImpl {
    [[nodiscard]] static const PanelImpl& instance() noexcept;

    cs2::CPanel2D::Create* create{ PanelPatterns::create() };
};
