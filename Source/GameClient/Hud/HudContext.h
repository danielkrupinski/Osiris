#pragma once

#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

template <typename HookContext>
struct HudContext {
    explicit HudContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto panel() noexcept
    {
        auto&& hud = hookContext.clientPatternSearchResults().template get<HudPanelPointer>();
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
    
private:
    HookContext& hookContext;
};
