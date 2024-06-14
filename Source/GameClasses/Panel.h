#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameDependencies/PanelDeps.h>

struct Panel {
    [[nodiscard]] static cs2::CPanel2D* create(const char* id, cs2::CUIPanel* parent) noexcept
    {
        if (PanelDeps::instance().create)
            return PanelDeps::instance().create(id, parent);
        return nullptr;
    }
};
