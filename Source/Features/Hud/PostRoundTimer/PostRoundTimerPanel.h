#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <Utils/StringBuilder.h>

#include "PostRoundTimerPanelFactory.h"

template <typename HookContext>
class PostRoundTimerPanel {
public:
    explicit PostRoundTimerPanel(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void showAndUpdate() const
    {
        countdownContainerPanel().show();

        auto&& countdownTextPanel_ = countdownTextPanel();
        updateCountdownTime(countdownTextPanel_);
        countdownTextPanel_.uiPanel().setColor(getColor());
    }

    void hide() const
    {
        countdownContainerPanel().hide();
    }

private:
    [[nodiscard]] auto getColor() const
    {
        switch (localPlayerTeamNumber()) {
        using enum TeamNumber;
        case TT: return cs2::kColorTeamTT;
        case CT: return cs2::kColorTeamCT;
        default: return cs2::kColorSilver;
        }
    }

    [[nodiscard]] decltype(auto) localPlayerTeamNumber() const
    {
        return hookContext.localPlayerController().teamNumber();
    }

    void updateCountdownTime(auto&& countdownTextPanel) const
    {
        const auto timeToRoundRestart = getTimeToRoundRestart();
        countdownTextPanel.setText(StringBuilderStorage<10>{}.builder().put(static_cast<int>(timeToRoundRestart), '.', static_cast<int>(timeToRoundRestart * 10) % 10).cstring());
    }

    [[nodiscard]] float getTimeToRoundRestart() const
    {
        if (auto&& timeToRoundRestart = hookContext.gameRules().timeToRoundRestart(); timeToRoundRestart.hasValue() && timeToRoundRestart.value() >= 0.0f)
            return timeToRoundRestart.value();
        return 0.0f;
    }

    [[nodiscard]] decltype(auto) countdownTextPanel() const
    {
        return countdownContainerPanel().children()[0].clientPanel().template as<PanoramaLabel>();
    }

    [[nodiscard]] decltype(auto) countdownContainerPanel() const
    {
        return hookContext.template make<PanelHandle>(state().countdownContainerPanelHandle).getOrInit(createCountdownContainerPanel());
    }

    [[nodiscard]] decltype(auto) createCountdownContainerPanel() const noexcept
    {
        return [this] {
            auto&& factory = hookContext.template make<PostRoundTimerPanelFactory>();
            auto&& containerPanel = factory.createCountdownContainerPanel(hookContext.hud().scoreAndTimeAndBomb());
            factory.createCountdownTextPanel(containerPanel);
            return containerPanel;
        };
    }

    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().hudFeaturesStates.postRoundTimerState;
    }

    HookContext& hookContext;
};
