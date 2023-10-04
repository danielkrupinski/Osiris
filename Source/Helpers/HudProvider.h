#pragma once

#include <CS2/Classes/Panorama.h>

#include <GameClasses/PanoramaUiPanel.h>
#include <MemoryPatterns/ClientPatterns.h>

struct HudProvider {
    [[nodiscard]] PanoramaUiPanel findChildInLayoutFile(const char* childId) const noexcept
    {
        if (hud && *hud)
            return PanoramaUiPanel{ (*hud)->uiPanel }.findChildInLayoutFile(childId);
        return PanoramaUiPanel{ nullptr };
    }

private:
    cs2::CPanel2D** hud{ ClientPatterns::hudPanel() };
};
