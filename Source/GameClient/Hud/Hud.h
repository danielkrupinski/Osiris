#pragma once

#include <CS2/Constants/PanelIDs.h>

#include "DeathNotices.h"

template <typename HookContext>
class Hud {
public:
    explicit Hud(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) deathNotices() noexcept
    {
        return deathNoticesPanelHandle().getOrInit(findVisibleDeathNoticesPanel()).template as<DeathNotices>();
    }

    [[nodiscard]] decltype(auto) timerTextPanel() noexcept
    {
        return timerTextPanelHandle().getOrInit(findTimerTextPanel());
    }

    [[nodiscard]] decltype(auto) getHudReticle() noexcept
    {
        return panel().findChildInLayoutFile(cs2::panel_id::HudReticle);
    }

    [[nodiscard]] decltype(auto) scoreAndTimeAndBomb() noexcept
    {
        return scoreAndTimeAndBombPanelHandle().getOrInit(findScoreAndTimeAndBombPanel());
    }

    [[nodiscard]] decltype(auto) bombStatus() noexcept
    {
        return bombStatusPanelHandle().getOrInit(findBombStatusPanel());
    }

    [[nodiscard]] decltype(auto) hudTeamCounter() noexcept
    {
        return panel().findChildInLayoutFile(cs2::panel_id::HudTeamCounter);
    }

    [[nodiscard]] decltype(auto) bombPlantedPanel() noexcept
    {
        return bombPlantedPanelHandle().getOrInit(findBombPlantedPanel());
    }

private:
    [[nodiscard]] decltype(auto) hudDeathNotice() noexcept
    {
        return panel().findChildInLayoutFile(cs2::panel_id::HudDeathNotice);
    }

    [[nodiscard]] auto findVisibleDeathNoticesPanel() noexcept
    {
        return [this] { 
            return hudDeathNotice().findChildInLayoutFile(cs2::panel_id::VisibleNotices);
        };
    }

    [[nodiscard]] auto findBombStatusPanel() noexcept
    {
        return [this] {
            resetBombStatusVisibility();
            return scoreAndTimeAndBomb().findChildInLayoutFile(cs2::panel_id::BombStatus);
        };
    }

    [[nodiscard]] auto findBombPlantedPanel() noexcept
    {
        return [this] {
            return bombStatus().findChildInLayoutFile(cs2::panel_id::BombPlanted);
        };
    }

    [[nodiscard]] auto findScoreAndTimeAndBombPanel() noexcept
    {
        return [this] {
            return hudTeamCounter().findChildInLayoutFile(cs2::panel_id::ScoreAndTimeAndBomb);
        };
    }

    [[nodiscard]] auto findTimerTextPanel() noexcept
    {
        return [this] {
            return scoreAndTimeAndBomb().findChildInLayoutFile(cs2::panel_id::TimerText);
        };
    }

    [[nodiscard]] auto panel() noexcept
    {
        auto&& hud = hookContext.patternSearchResults().template get<HudPanelPointer>();
        if (hud && *hud)
            return hookContext.template make<PanoramaUiPanel>((*hud)->uiPanel);
        return hookContext.template make<PanoramaUiPanel>(nullptr);
    }

    [[nodiscard]] auto deathNoticesPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.hudState().deathNoticesPanelHandle);
    }

    [[nodiscard]] auto scoreAndTimeAndBombPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.hudState().scoreAndTimeAndBombPanelHandle);
    }

    [[nodiscard]] auto bombStatusPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.hudState().bombStatusPanelHandle);
    }

    [[nodiscard]] auto bombPlantedPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.hudState().bombPlantedPanelHandle);
    }

    [[nodiscard]] auto timerTextPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.hudState().timerTextPanelHandle);
    }

    void resetBombStatusVisibility() noexcept
    {
        hookContext.bombStatusPanelState().resetVisibility();
    }

    HookContext& hookContext;
};
