#pragma once

#include <CS2/Classes/Panorama.h>

struct PanelPatterns {
    [[nodiscard]] cs2::CPanel2D::Create* create() const noexcept;
};
