#pragma once

#include <CS2/Classes/Panorama.h>
#include "PanelStyleSetter.h"

struct PanelConfigurator {
    [[nodiscard]] PanelStyleSetter panelStyle(cs2::CPanelStyle& panelStyle) const noexcept
    {
        return PanelStyleSetter{panelStyle, propertyFactory};
    }

    PanelStylePropertyFactory propertyFactory;
};
