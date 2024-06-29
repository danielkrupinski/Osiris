#pragma once

#include <Helpers/PanoramaPanelPointer.h>

struct HudDeps {
    template <typename ClientPatterns>
    explicit HudDeps(const ClientPatterns& clientPatterns) noexcept
        : hud{clientPatterns.hudPanel()}
    {
    }

    [[nodiscard]] static HudDeps& instance() noexcept;

    cs2::CPanel2D** hud;
    PanoramaPanelPointer deathNoticesPointer;
    PanoramaPanelPointer scoreAndTimeAndBombPanel;
    PanoramaPanelPointer bombStatusPanel;
};
