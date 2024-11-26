#pragma once

#include <CS2/Classes/CGameSceneNode.h>
#include <CS2/Classes/ConVarTypes.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Constants/ColorConstants.h>
#include <Entities/PlayerPawn.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/LifeState.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/FeatureToggle.h>
#include <FeatureHelpers/TeamNumber.h>
#include <GameClasses/PanoramaLabel.h>
#include <GameClasses/PanoramaImagePanel.h>
#include <Hooks/ViewRenderHook.h>
#include <Utils/ColorUtils.h>
#include <Utils/CString.h>

#include "PlayerInfoPanel.h"

#include "PlayerInfoInWorldPanelFactory.h"
#include "PlayerInfoInWorldState.h"

struct PlayerPositionToggle : public FeatureToggle<PlayerPositionToggle> {
    explicit PlayerPositionToggle(PlayerInfoInWorldState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showPlayerPosition;
    }

    PlayerInfoInWorldState& state;
};

struct PlayerPositionArrowColorToggle {
    explicit PlayerPositionArrowColorToggle(PlayerPositionArrowColorType& color) noexcept
        : color{color}
    {
    }

    void update(char option) const noexcept
    {
        switch (option) {
        case '0': color = PlayerPositionArrowColorType::PlayerOrTeamColor; break;
        case '1': color = PlayerPositionArrowColorType::TeamColor; break;
        }
    }

private:
    PlayerPositionArrowColorType& color;
};

struct PlayerHealthToggle : public FeatureToggle<PlayerHealthToggle> {
    explicit PlayerHealthToggle(PlayerInfoInWorldState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showPlayerHealth;
    }

    PlayerInfoInWorldState& state;
};

struct PlayerHealthTextColorToggle {
    explicit PlayerHealthTextColorToggle(PlayerHealthTextColor& color) noexcept
        : color{color}
    {
    }

    void update(char option) const noexcept
    {
        switch (option) {
        case '0': color = PlayerHealthTextColor::HealthBased; break;
        case '1': color = PlayerHealthTextColor::White; break;
        }
    }

private:
    PlayerHealthTextColor& color;
};

struct PlayerActiveWeaponToggle : public FeatureToggle<PlayerActiveWeaponToggle> {
    explicit PlayerActiveWeaponToggle(PlayerInfoInWorldState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showPlayerActiveWeapon;
    }

    PlayerInfoInWorldState& state;
};

struct PlayerActiveWeaponAmmoToggle : public FeatureToggle<PlayerActiveWeaponAmmoToggle> {
    explicit PlayerActiveWeaponAmmoToggle(PlayerInfoInWorldState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showPlayerActiveWeaponAmmo;
    }

    PlayerInfoInWorldState& state;
};

struct BombIconToggle : public FeatureToggle<BombIconToggle> {
    explicit BombIconToggle(PlayerInfoInWorldState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showBombCarrierIcon;
    }

    PlayerInfoInWorldState& state;
};

struct BombPlantingIconToggle : public FeatureToggle<BombPlantingIconToggle> {
    explicit BombPlantingIconToggle(PlayerInfoInWorldState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.showBombPlantingIcon;
    }

    PlayerInfoInWorldState& state;
};


template <typename IconPanel>
struct PlayerStateIconToggle {
    explicit PlayerStateIconToggle(PlayerStateIconsToShow& playerStateIconsToShow) noexcept
        : playerStateIconsToShow{playerStateIconsToShow}
    {
    }

    void update(char option) noexcept
    {
        switch (option) {
        case '0': playerStateIconsToShow.set<IconPanel>(); break;
        case '1': playerStateIconsToShow.unset<IconPanel>(); break;
        }
    }

    PlayerStateIconsToShow& playerStateIconsToShow;
};

using PlayerDefuseIconToggle = PlayerStateIconToggle<DefuseIconPanel>;
using HostagePickupIconToggle = PlayerStateIconToggle<HostagePickupPanel>;
using HostageRescueIconToggle = PlayerStateIconToggle<HostageRescuePanel>;
using BlindedIconToggle = PlayerStateIconToggle<BlindedIconPanel>;

template <typename HookContext>
struct PlayerInfoInWorldToggle : FeatureToggleOff<PlayerInfoInWorldToggle<HookContext>> {
    PlayerInfoInWorldToggle(PlayerInfoInWorldState& state, HookContext& hookContext, ViewRenderHook& viewRenderHook) noexcept
        : state{state}
        , hookContext{hookContext}
        , viewRenderHook{viewRenderHook}
    {
    }

    void update(char option) noexcept
    {
        switch (option) {
        case '0': this->enable(); state.showOnlyEnemies = true; break;
        case '1': this->enable(); state.showOnlyEnemies = false; break;
        case '2': this->disable(); break;
        }
    }

    [[nodiscard]] auto& enabledVariable(typename PlayerInfoInWorldToggle::ToggleMethod) const noexcept
    {
        return state.enabled;
    }

    void onDisable(typename PlayerInfoInWorldToggle::ToggleMethod) noexcept
    {
        if (const auto containerPanel{hookContext.template make<InWorldPanelContainer>().get()}) {
            if (const auto containerPanelChildren{containerPanel.children().vector})
                hideRemainingPanels(HudInWorldPanels{*containerPanelChildren}, 0);
        }
    }

private:
    void hideRemainingPanels(HudInWorldPanels inWorldPanels, std::size_t firstIndexToHide) const noexcept
    {
        for (std::size_t i = firstIndexToHide; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[i], hookContext)}) {
                panel.setOpacity(0.0f);
            }
        }
    }

    PlayerInfoInWorldState& state;
    HookContext& hookContext;
    ViewRenderHook& viewRenderHook;
};

#include "PlayerInfoInWorldCondition.h"
#include "PlayerInfoInWorldContext.h"

template <typename HookContext>
class PlayerInfoInWorld {
public:
    PlayerInfoInWorld(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void drawPlayerInformation(auto&& playerPawn) noexcept
    {
        auto&& condition = context().condition();
        if (!condition.shouldRun() || !condition.shouldDrawInfoOnPawn(playerPawn))
            return;

        const auto absOrigin = playerPawn.absOrigin();
        if (!absOrigin.hasValue())
            return;

        const auto positionInClipSpace = hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(absOrigin.value());
        if (!positionInClipSpace.onScreen())
            return;

        const auto containerPanel{hookContext.template make<InWorldPanelContainer>().get()};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children().vector};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        if (state().containerPanelHandle != hookContext.template make<PanoramaUiEngine>().getPanelHandle(containerPanel)) {
            state().panelIndices.clear();
            state().containerPanelHandle = hookContext.template make<PanoramaUiEngine>().getPanelHandle(containerPanel);
        }

        const auto panel = getPanel(containerPanel, panels);
        if (!panel)
            return;

        PlayerInfoPanel playerInformationPanel{hookContext, panel};
        playerInformationPanel.drawPlayerInfo(playerPawn);
        playerInformationPanel.updatePosition(absOrigin.value());

        ++currentIndex;
    }

    void hideUnusedPanels() const noexcept
    {
        const auto containerPanel{hookContext.template make<InWorldPanelContainer>().get()};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children().vector};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        for (std::size_t i = currentIndex; i < state().panelIndices.getSize(); ++i) {
            if (const auto panel{panels.getPanel(state().panelIndices[i], hookContext)}) {
                panel.setOpacity(0.0f);
            }
        }
    }

private:
    [[nodiscard]] decltype(auto) context() const noexcept
    {
        return hookContext.template make<PlayerInfoInWorldContext>();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState;
    }

    [[nodiscard]] PanoramaUiPanel<PanoramaUiPanelContext<HookContext>> getPanel(auto&& containerPanel, HudInWorldPanels inWorldPanels) const noexcept
    {
        if (currentIndex < state().panelIndices.getSize()) {
            if (const auto panel{inWorldPanels.getPanel(state().panelIndices[currentIndex], hookContext)})
                return panel;
            state().panelIndices.fastRemoveAt(currentIndex);
        }
        if (const auto panel{createPlayerInfoPanel(containerPanel)}) {
            state().panelIndices.pushBack(inWorldPanels.getIndexOfLastPanel());
            return panel;
        }
        return PanoramaUiPanel{PanoramaUiPanelContext{hookContext, nullptr}};
    }

    [[nodiscard]] decltype(auto) createPlayerInfoPanel(auto&& parent) const noexcept
    {
        auto&& factory = hookContext.template make<PlayerInfoInWorldPanelFactory>();
        auto&& containerPanel = factory.createContainerPanel(parent);
        createPanels(std::type_identity<PlayerInfoPanelTypes<HookContext>>{}, factory, containerPanel);
        return utils::lvalue<decltype(containerPanel)>(containerPanel);
    }

    template <typename... PanelTypes>
    void createPanels(std::type_identity<std::tuple<PanelTypes...>>, auto&& factory, auto&& containerPanel) const noexcept
    {
        (factory.createPanel(std::type_identity<PanelTypes>{}, containerPanel), ...);
    }

    HookContext& hookContext;
    std::size_t currentIndex{0};
};
