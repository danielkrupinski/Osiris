#pragma once

#include <CS2/Classes/Panorama.h>
#include <Common/Visibility.h>

struct BombStatusPanelState {
    cs2::PanelHandle invisiblePanelHandle;
    Visibility visibility{Visibility::Visible};

    void resetVisibility() noexcept
    {
        visibility = Visibility::Visible;
    }
};
