#pragma once

#include <utility>

#include <Common/Visibility.h>
#include <HookContext/HookContextMacros.h>
#include "BombTimerConfigVariables.h"
#include "BombTimerPanel.h"

template <typename HookContext>
class BombTimer {
public:
    explicit BombTimer(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] Visibility update() const
    {
        if (!enabled())
            return Visibility::Hidden;

        auto&& plantedC4 = hookContext.plantedC4();
        if (shouldShowBombTimer(plantedC4)) {
            bombTimerPanel().showAndUpdate(plantedC4);
            return Visibility::Visible;
        } else {
            bombTimerPanel().hide();
            return Visibility::Hidden;
        }
    }

    void forceHide() const
    {
        if (enabled())
            bombTimerPanel().hide();
    }

    void onDisable() const
    {
        bombTimerPanel().hide();
    }

    void onUnload() const
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(state().bombTimerContainerPanelHandle);
    }

private:
    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(BombTimerEnabled);
    }

    [[nodiscard]] bool shouldShowBombTimer(auto&& plantedC4) const
    {
        return bombPlantedPanel().isVisible().valueOr(true)
            && plantedC4
            && plantedC4.isTicking().valueOr(true)
            && plantedC4.getTimeToExplosion().greaterThan(0.0f).valueOr(false);
    }

    [[nodiscard]] decltype(auto) bombTimerPanel() const
    {
        return hookContext.template make<BombTimerPanel>();
    }

    [[nodiscard]] decltype(auto) bombPlantedPanel() const
    {
        return hookContext.hud().bombPlantedPanel();
    }

    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().hudFeaturesStates.bombTimerState;
    }

    HookContext& hookContext;
};
