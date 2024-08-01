#pragma once

#include <CS2/Classes/Panorama.h>

struct HudDeps {
    template <typename ClientPatterns>
    explicit HudDeps(const ClientPatterns& clientPatterns) noexcept
        : hud{clientPatterns.hudPanel()}
    {
    }

    [[nodiscard]] static HudDeps& instance() noexcept;

    cs2::CPanel2D** hud;
    cs2::PanelHandle deathNoticesPanelHandle;
    cs2::PanelHandle scoreAndTimeAndBombPanelHandle;
    cs2::PanelHandle bombStatusPanelHandle;
    cs2::PanelHandle bombPlantedPanelHandle;
};
