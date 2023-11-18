#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>

#include <GameClasses/PanoramaUiPanel.h>
#include <MemoryPatterns/ClientPatterns.h>

struct HudProvider {
    [[nodiscard]] PanoramaUiPanel findChildInLayoutFile(const char* childId) const noexcept
    {
        if (hud && *hud)
            return PanoramaUiPanel{ (*hud)->uiPanel }.findChildInLayoutFile(childId);
        return PanoramaUiPanel{ nullptr };
    }

    [[nodiscard]] cs2::CUIPanel* getHudReticle() const noexcept
    {
        return findChildInLayoutFile(cs2::HudReticle);
    }

private:
    cs2::CPanel2D** hud{ ClientPatterns::hudPanel() };
};
