#pragma once

#include <cassert>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>

#include "DeathNotices.h"

template <typename Dependencies>
struct Hud {
    Hud(Dependencies dependencies) noexcept
        : dependencies{dependencies}
    {
    }

    [[nodiscard]] auto deathNotices() const noexcept
    {
        return DeathNotices{getDeathNoticesPanel(), dependencies};
    }

    [[nodiscard]] cs2::CUIPanel* getHudReticle() const noexcept
    {
        return findChildInLayoutFile(cs2::HudReticle);
    }

    [[nodiscard]] PanoramaUiPanel hudDeathNotice() const noexcept
    {
        return findChildInLayoutFile(cs2::HudDeathNotice);
    }

    [[nodiscard]] PanoramaUiPanel getDeathNoticesPanel() const noexcept
    {
        auto& deps = HudDeps::instance();

        if (const auto deathNoticesPanel = deps.deathNoticesPointer.get())
            return deathNoticesPanel;

        if (const auto hudDeathNotice_ = hudDeathNotice())
            deps.deathNoticesPointer = hudDeathNotice_.findChildInLayoutFile(cs2::VisibleNotices);

        return deps.deathNoticesPointer.get();
    }

    [[nodiscard]] PanoramaUiPanel scoreAndTimeAndBomb() const noexcept
    {
        auto& deps = HudDeps::instance();

        if (const auto scoreAndTimeAndBombPanel = deps.scoreAndTimeAndBombPanel.get())
            return scoreAndTimeAndBombPanel;

        if (const auto hudTeamCounter_ = hudTeamCounter())
            deps.scoreAndTimeAndBombPanel = hudTeamCounter_.findChildInLayoutFile(cs2::ScoreAndTimeAndBomb);

        return deps.scoreAndTimeAndBombPanel.get();
    }

    [[nodiscard]] PanoramaUiPanel bombStatus() const noexcept
    {
        auto& deps = HudDeps::instance();

        if (const auto bombStatusPanel = deps.bombStatusPanel.get())
            return bombStatusPanel;

        if (const auto scoreAndTimeAndBomb_ = scoreAndTimeAndBomb())
            deps.bombStatusPanel = scoreAndTimeAndBomb_.findChildInLayoutFile("BombStatus");

        return deps.bombStatusPanel.get();
    }

    [[nodiscard]] PanoramaUiPanel hudTeamCounter() const noexcept
    {
        return findChildInLayoutFile(cs2::HudTeamCounter);
    }

private:
    [[nodiscard]] PanoramaUiPanel findChildInLayoutFile(const char* childId) const noexcept
    {
        const auto hud = HudDeps::instance().hud;
        if (hud && *hud)
            return PanoramaUiPanel{(*hud)->uiPanel}.findChildInLayoutFile(childId);
        return PanoramaUiPanel{nullptr};
    }

    Dependencies dependencies;
};
