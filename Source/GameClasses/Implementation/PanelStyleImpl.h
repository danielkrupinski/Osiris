#pragma once

#include <MemoryPatterns/PanelStylePatterns.h>

struct PanelStyleImpl {
    [[nodiscard]] static const PanelStyleImpl& instance() noexcept;

    cs2::CPanelStyle::SetProperty* setProperty{PanelStylePatterns::setProperty()};
};
