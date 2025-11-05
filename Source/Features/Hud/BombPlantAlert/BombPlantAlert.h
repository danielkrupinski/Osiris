#pragma once

#include "BombPlantAlertConfigVariables.h"
#include "BombPlantAlertPanelFactory.h"
#include "BombPlantAlertParams.h"
#include <CS2/Constants/BombsiteIndex.h>
#include <CS2/Constants/IconURLs.h>
#include <Common/Visibility.h>
#include <GameClient/Entities/C4.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <HookContext/HookContextMacros.h>
#include <Utils/StringBuilder.h>

template <typename HookContext>
class BombPlantAlert {
public:
    explicit BombPlantAlert(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] Visibility show(auto&& playerPawn) const
    {
        if (!GET_CONFIG_VAR(BombPlantAlertEnabled))
            return Visibility::Hidden;

        auto&& bomb = playerPawn.getActiveWeapon().template cast<C4>();
        if (!shouldShowPlantAlert(bomb))
            return Visibility::Hidden;

        const auto canBePlantedBeforeRoundEnd = canFinishArmingBeforeRoundEnd(bomb);
        updateContainerPanel(canBePlantedBeforeRoundEnd);
        updateBombsiteIcon(bomb.nearestBombsiteIndex());
        updateTimerPanel(canBePlantedBeforeRoundEnd, bomb);

        return Visibility::Visible;
    }

    void hide() const
    {
        if (GET_CONFIG_VAR(BombPlantAlertEnabled))
            getPanel(state().containerPanelHandle).setVisible(false);
    }

    void onDisable() const
    {
        getPanel(state().containerPanelHandle).setVisible(false);
    }

    void onUnload() const
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(state().containerPanelHandle);
    }

private:
    [[nodiscard]] Optional<bool> canFinishArmingBeforeRoundEnd(auto&& bomb) const
    {
        auto&& gameRules = hookContext.gameRules();
        if (gameRules.isRoundOver().valueOr(false))
            return false;
        return bomb.armingEndTime().lessThan(gameRules.roundEndTime());
    }

    [[nodiscard]] bool shouldShowPlantAlert(auto&& bomb) const
    {
        return bomb && bomb.isBeingPlanted().valueOr(false) && bomb.timeToArmingEnd().greaterThan(0.0f).valueOr(false);
    }

    void updateContainerPanel(auto canBePlantedBeforeRoundEnd) const
    {
        auto&& panel = hookContext.template make<PanelHandle>(state().containerPanelHandle).getOrInit(createPanel());
        panel.setVisible(true);
        panel.setBackgroundColor(backgroundColor(canBePlantedBeforeRoundEnd));
    }

    void updateBombsiteIcon(auto bombsiteIndex) const
    {
        getPanel(state().bombsiteAIconPanelHandle).setVisible(bombsiteIndex == cs2::BombsiteIndex::BombsiteA);
        getPanel(state().bombsiteBIconPanelHandle).setVisible(bombsiteIndex == cs2::BombsiteIndex::BombsiteB);
    }

    void updateTimerPanel(auto canBePlantedBeforeRoundEnd, auto&& bomb) const
    {
        auto&& timerPanel = getPanel(state().timerPanelHandle);
        const auto timeToArmingEnd = bomb.timeToArmingEnd();
        if (timeToArmingEnd.hasValue()) {
            timerPanel.clientPanel().template as<PanoramaLabel>().setText(StringBuilderStorage<10>{}.builder().put(
                static_cast<int>(timeToArmingEnd.value()), '.', static_cast<int>(timeToArmingEnd.value() * 10) % 10).cstring());
        }
        timerPanel.setColor(timerColor(canBePlantedBeforeRoundEnd));
    }

    [[nodiscard]] decltype(auto) getPanel(auto panelHandle) const
    {
        return hookContext.template make<PanoramaUiEngine>().getPanelFromHandle(panelHandle);
    }

    [[nodiscard]] static cs2::Color backgroundColor(auto canBePlantedBeforeRoundEnd) noexcept
    {
        using namespace bomb_plant_alert_params;
        if (canBePlantedBeforeRoundEnd.hasValue())
            return canBePlantedBeforeRoundEnd.value() ? kPositiveBackgroundColor : kNegativeBackgroundColor;
        return kFallbackBackgroundColor;
    }

    [[nodiscard]] static cs2::Color timerColor(auto canBePlantedBeforeRoundEnd) noexcept
    {
        using namespace bomb_plant_alert_params;
        if (canBePlantedBeforeRoundEnd.hasValue())
            return canBePlantedBeforeRoundEnd.value() ? kPositiveTimerColor : kNegativeTimerColor;
        return kFallbackTimerColor;
    }

    [[nodiscard]] auto createPanel() const noexcept
    {
        return [this] () -> decltype(auto) {
            auto&& factory = hookContext.template make<BombPlantAlertPanelFactory>();
            auto&& container = factory.createContainerPanel(hookContext.hud().scoreAndTimeAndBomb());
            state().containerPanelHandle = container.getHandle();
            state().bombsiteAIconPanelHandle = factory.createBombsiteIconPanel(container, cs2::kBombsiteAPlantingIconUrl).getHandle();
            state().bombsiteBIconPanelHandle = factory.createBombsiteIconPanel(container, cs2::kBombsiteBPlantingIconUrl).getHandle();
            state().timerPanelHandle = factory.createTimerPanel(container).getHandle();
            return utils::lvalue<decltype(container)>(container);
        };
    }

    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().hudFeaturesStates.bombPlantAlertState;
    }

    HookContext& hookContext;
};
