#pragma once

#include <GameClasses/PanelHandle.h>

template <typename HookContext>
struct HudContext {
    explicit HudContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto panel() noexcept
    {
        auto&& hud = hookContext.gameDependencies().hudDeps.hud;
        if (hud && *hud)
            return hookContext.template make<PanoramaUiPanel>((*hud)->uiPanel);
        return hookContext.template make<PanoramaUiPanel>(nullptr);
    }

    [[nodiscard]] auto deathNoticesPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.gameDependencies().hudDeps.deathNoticesPanelHandle);
    }

    [[nodiscard]] auto scoreAndTimeAndBombPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.gameDependencies().hudDeps.scoreAndTimeAndBombPanelHandle);
    }

    [[nodiscard]] auto bombStatusPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.gameDependencies().hudDeps.bombStatusPanelHandle);
    }

    [[nodiscard]] auto bombPlantedPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.gameDependencies().hudDeps.bombPlantedPanelHandle);
    }

    [[nodiscard]] auto timerTextPanelHandle() noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.gameDependencies().hudDeps.timerTextPanelHandle);
    }

    void resetBombStatusVisibility() noexcept
    {
        hookContext.bombStatusPanelState().resetVisibility();
    }
    
private:
    HookContext& hookContext;
};
