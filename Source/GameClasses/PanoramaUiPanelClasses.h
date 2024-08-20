#pragma once

#include <CS2/Panorama/CPanoramaSymbol.h>
#include <CS2/Panorama/CUIPanel.h>

struct PanoramaUiPanelClasses {
    [[nodiscard]] bool hasClass(cs2::CPanoramaSymbol className) const noexcept
    {
        if (!classes)
            return false;

        for (int i = 0; i < classes->size; ++i) {
            if (classes->memory[i] == className)
                return true;
        }
        return false;
    }

    const cs2::CUIPanel::classesVector* classes;
};
