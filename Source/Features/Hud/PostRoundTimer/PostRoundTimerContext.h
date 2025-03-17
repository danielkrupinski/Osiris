#pragma once

#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanelHandle.h>

#include "PostRoundTimerCondition.h"
#include "PostRoundTimerPanel.h"
#include "PostRoundTimerPanelFactory.h"

template <typename HookContext>
struct PostRoundTimerContext {
    explicit PostRoundTimerContext(HookContext& hookContext) noexcept
        : _hookContext{hookContext}
    {
    }

    [[nodiscard]] bool isGameRoundTimeVisible() const noexcept
    {
        return _hookContext.hud().timerTextPanel().isVisible().valueOr(false);
    }

    [[nodiscard]] decltype(auto) gameRules() const noexcept
    {
        return _hookContext.gameRules();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return _hookContext.featuresStates().hudFeaturesStates.postRoundTimerState;
    }

    [[nodiscard]] decltype(auto) countdownTextPanel() const noexcept
    {
        return countdownContainerPanel().children()[0].clientPanel().template as<PanoramaLabel>();
    }

    [[nodiscard]] decltype(auto) countdownContainerPanel() const noexcept
    {
        return countdownContainerPanelHandle().getOrInit(createCountdownContainerPanel());
    }

    [[nodiscard]] auto condition() const noexcept
    {
        return PostRoundTimerCondition{*this};
    }

    [[nodiscard]] auto postRoundTimerPanel() const noexcept
    {
        return PostRoundTimerPanel{*this};
    }

    [[nodiscard]] decltype(auto) localPlayerTeamNumber() const noexcept
    {
        return _hookContext.localPlayerController().teamNumber();
    }

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return _hookContext.config();
    }

private:
    [[nodiscard]] decltype(auto) countdownContainerPanelHandle() const noexcept
    {
        return _hookContext.template make<PanelHandle>(state().countdownContainerPanelHandle);
    }

    [[nodiscard]] decltype(auto) createCountdownContainerPanel() const noexcept
    {
        return [this] {
            auto&& factory = _hookContext.template make<PostRoundTimerPanelFactory>();
            auto&& containerPanel = factory.createCountdownContainerPanel(_hookContext.hud().scoreAndTimeAndBomb());
            factory.createCountdownTextPanel(containerPanel);
            return containerPanel;
        };
    }

    HookContext& _hookContext;
};
