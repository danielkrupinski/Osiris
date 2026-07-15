#pragma once

#include <CS2/Panorama/CUIEngine.h>
#include <CS2/Panorama/PanelHandle.h>
#include <Features/Visuals/GrenadePrediction/Trajectory.h>

struct GrenadePredictionState {
    cs2::PanelHandle containerPanelHandle{};
    Trajectory cachedTrajectory{};
    Trajectory tempTrajectory{};

    // Cache throw strength while pin is pulled to prevent flicker on release.
    // When m_bPinPulled goes false (release animation), we keep using the last
    // known throw strength instead of snapping to 1.0f.
    float cachedThrowStrength{1.0f};
    const void* cachedThrowStrengthWeapon{};

};
