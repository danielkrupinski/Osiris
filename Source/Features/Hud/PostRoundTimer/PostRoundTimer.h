#pragma once

#include <utility>
#include <Common/Visibility.h>
#include <HookContext/HookContextMacros.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanelHandle.h>

#include "PostRoundTimerConfigVariables.h"
#include "PostRoundTimerPanel.h"
#include "PostRoundTimerPanelFactory.h"

template <typename HookContext>
class PostRoundTimer {
public:
    explicit PostRoundTimer(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] Visibility update() const
    {
        using enum Visibility;

        if (!enabled())
            return Hidden;

        if (shouldShowPostRoundTimer()) {
            postRoundTimerPanel().showAndUpdate();
            return Visible;
        } else {
            postRoundTimerPanel().hide();
            return Hidden;
        }
    }

    void onDisable() const
    {
        postRoundTimerPanel().hide();
    }

    void onUnload() const
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(state().countdownContainerPanelHandle);
    }

private:
    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(PostRoundTimerEnabled);
    }

    [[nodiscard]] bool shouldShowPostRoundTimer() const
    {
        return hookContext.gameRules().hasScheduledRoundRestart() && !isGameRoundTimeVisible();
    }

    [[nodiscard]] bool isGameRoundTimeVisible() const
    {
        return hookContext.hud().timerTextPanel().isVisible().valueOr(false);
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().hudFeaturesStates.postRoundTimerState;
    }

    [[nodiscard]] decltype(auto) postRoundTimerPanel() const
    {
        return hookContext.template make<PostRoundTimerPanel>();
    }

    HookContext& hookContext;
};
