#pragma once

#include <CS2/Classes/Panorama.h>
#include "Implementation/PanelImpl.h"

struct Panel {
    [[nodiscard]] static cs2::CPanel2D* create(const char* id, cs2::CUIPanel* parent) noexcept
    {
        if (PanelImpl::instance().create)
            return PanelImpl::instance().create(id, parent);
        return nullptr;
    }
};
